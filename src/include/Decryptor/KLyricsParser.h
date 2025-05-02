//
// Created by 周俊杰 on 2025/3/30.
//

#ifndef KLYRICSPARSER_H
#define KLYRICSPARSER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <regex>
#include <chrono> // 用于高精度时钟
//#include "Decryptor/KRCDecryptor.h"
#include "MusicPlayer.h"
#include <QObject>
struct K_Lyric {
    std::string text;
    int start_time;
    int duration;
};

struct K_LyricLine {
    std::vector<K_Lyric> lyrics;
    int line_start_time; // 每行的开始时间
    int line_duration;   // 每行的持续时间
};

struct K_LyricData {
    std::vector<K_LyricLine> lines;
};

class K_LyricParser  : public QObject {
    Q_OBJECT
public:
    K_LyricData lyricData;
	QJsonObject music_pos;
	int max_time;
	std::map<std::tuple<int, int, int, int>, std::pair<int, int>> timestampMap;
	void parseLyrics(const std::string& data) {
		try {
			// 清空之前的歌词
			lyricData.lines.clear();

			std::istringstream stream(data);
			std::string line;

			std::regex linePattern(R"(\[(\d+),(\d+)\](.*))");
			std::regex lyricPattern(R"(<(\d+),(\d+),(\d+)>([^<]+))");

			while (std::getline(stream, line)) {
				std::smatch lineMatch;
				if (std::regex_search(line, lineMatch, linePattern)) {
					K_LyricLine lyricLine;
					lyricLine.line_start_time = std::stoi(lineMatch[1].str());
					lyricLine.line_duration = std::stoi(lineMatch[2].str());

					std::string::const_iterator searchStart(lineMatch[3].first);
					while (std::regex_search(searchStart, line.cend(), lineMatch, lyricPattern)) {
						K_Lyric lyric;
						lyric.start_time = std::stoi(lineMatch[1].str());
						lyric.duration = std::stoi(lineMatch[2].str());
						lyric.text = lineMatch[4].str();

						lyricLine.lyrics.emplace_back(lyric);

						searchStart = lineMatch.suffix().first;
					}

					lyricData.lines.emplace_back(lyricLine);
				}
			}

			if (!lyricData.lines.empty()) {
				max_time = lyricData.lines.back().line_start_time + lyricData.lines.back().line_duration - 1;
			}

			buildTimestampMap(lyricData);
		} catch (const std::exception& e) {
			// 捕获并处理异常
			std::cerr << "Error in parseLyrics function: " << e.what() << std::endl;
			// 清空数据以防异常后数据不一致
			lyricData.lines.clear();
			max_time = 0;
			timestampMap.clear();
		}
	}

	std::map<std::tuple<int, int, int, int>, std::pair<int, int>> buildTimestampMap(const K_LyricData& lyricData) {
        try {
            this->timestampMap.clear();
            std::map<std::tuple<int, int, int, int>, std::pair<int, int>> timestampMap;

            for (int lineIndex = 0; lineIndex < lyricData.lines.size(); ++lineIndex) {
                for (int lyricIndex = 0; lyricIndex < lyricData.lines[lineIndex].lyrics.size(); ++lyricIndex) {
                    const K_Lyric& lyric = lyricData.lines[lineIndex].lyrics[lyricIndex];
                    int startTime = lyric.start_time;
                    int endTime = startTime + lyric.duration;
                    int lineStartTime = lyricData.lines[lineIndex].line_start_time;
                    int lineEndTime = lineStartTime + lyricData.lines[lineIndex].line_duration - 1;

                    if (lyricIndex == lyricData.lines[lineIndex].lyrics.size() - 1) {
                        if (lineIndex == lyricData.lines.size() - 1) {
                            // Handle the last line and last lyric
                            this->timestampMap[std::make_tuple(lineStartTime, lineEndTime, startTime, endTime)] = {lineIndex, lyricIndex};
                        } else {
                            // Use the start time of the next line
                            int nextLineStartTime = lyricData.lines[lineIndex + 1].line_start_time;
                            this->timestampMap[std::make_tuple(lineStartTime, lineEndTime, startTime, nextLineStartTime - lineStartTime - 1)] = {lineIndex, lyricIndex};
                        }
                    } else {
                        this->timestampMap[std::make_tuple(lineStartTime, lineEndTime, startTime, endTime)] = {lineIndex, lyricIndex};
                    }
                }
            }

            return this->timestampMap;
        } catch (const std::exception& e) {
            std::cerr << "Error in buildTimestampMap function: " << e.what() << std::endl;
            this->timestampMap.clear();
            return this->timestampMap;
        }
    }

    QJsonObject findLyricAtTime(int time) {
        try {
            if (lyricData.lines.empty()) {
                std::cerr << "Error: Lyric data is empty." << std::endl;
                music_pos["error"] = "Lyric data is empty";
                return music_pos;
            }

            for (const auto& entry : this->timestampMap) {
                const auto& range = entry.first; // 范围 {lineStartTime, lineEndTime, startTime, endTime}
                const auto& indices = entry.second; // {lineIndex, lyricIndex}

                if (time >= std::get<0>(range) && time <= (std::get<0>(range) + std::get<3>(range))) {
                    int lineIndex = indices.first;
                    int lyricIndex = indices.second;
                    const K_Lyric& lyric = this->lyricData.lines[lineIndex].lyrics[lyricIndex];
                    float percent = static_cast<float>(time - this->lyricData.lines[lineIndex].line_start_time - this->lyricData.lines[lineIndex].lyrics[lyricIndex].start_time) / static_cast<float>(this->lyricData.lines[lineIndex].lyrics[lyricIndex].duration);

                    music_pos["lineIndex"] = qint16(lineIndex);
                    music_pos["lyricIndex"] = qint16(lyricIndex);
                    music_pos["percent"] = percent;
                    return music_pos;
                }
            }

            std::cerr << "No lyric found at time " << time << std::endl;
            if (time >= this->max_time) {
                music_pos["lineIndex"] = qint16(this->lyricData.lines.size() - 1);
                music_pos["lyricIndex"] = qint16(this->lyricData.lines[this->lyricData.lines.size() - 1].lyrics.size() - 1);
                music_pos["percent"] = 1;
                return music_pos;
            }

            return music_pos;
        } catch (const std::exception& e) {
            std::cerr << "Error in findLyricAtTime function: " << e.what() << std::endl;
            music_pos["error"] = QString::fromStdString(e.what());
            return music_pos;
        }
    }

public slots:
	// 获取歌词
	QList<QList<QJsonObject>> Q_getLyrics() {
    	QList<QList<QJsonObject>> q_klyrics;
    	QList<QJsonObject> line;
    	QJsonObject music_detail_info;
    	for (int lineIndex = 0; lineIndex < lyricData.lines.size(); ++lineIndex) {
    		for (int lyricIndex = 0; lyricIndex < lyricData.lines[lineIndex].lyrics.size(); ++lyricIndex) {
    			music_detail_info["index"] = lyricIndex;
    			music_detail_info["content"] = QString::fromStdString(lyricData.lines[lineIndex].lyrics[lyricIndex].text);
    			line.append(music_detail_info);
    		}
    		q_klyrics.append(line);
    		line.clear(); // 清空当前行的数据，为下一行做准备
    	}

    	// for (int i = 0; i < q_klyrics.size(); i++) {
    	// 	for (int j = 0; j < q_klyrics[i].size(); j++) {
    	// 		std::cout << "index: " << q_klyrics[i][j]["index"].toInt() << " content: " << q_klyrics[i][j]["content"].toString().toStdString() << std::endl;
    	// 	}
    	// }

    	return q_klyrics;
    }

};

/*

int main() {
    std::string data = R"([0,17600]<0,1173,0>三<1173,1173,0>国<2346,1174,0>恋<3520,1173,0> <4693,1173,0>(<5866,1174,0>女<7040,1173,0>版<8213,1173,0>)<9386,1174,0> <10560,1173,0>-<11733,1173,0> <12906,1174,0>黎<14080,1173,0>林<15253,1173,0>添<16426,1174,0>娇
[17601,7040]<0,1173,0>词<1174,1173,0>：<2347,1173,0>Tank/<3521,1173,0>颜<4694,1173,0>玺<5868,1173,0>轩
[24642,3519]<0,1173,0>曲<1173,1173,0>：<2347,1173,0>Tank
[28162,4973]<0,1173,0>原<1174,1173,0>唱<2347,1173,0>：<3521,1453,0>Tank
[33136,2053]<0,335,0>将<335,173,0>军<508,0,0> <797,168,0>北<965,200,0>方<1165,167,0>苍<1332,251,0>狼<1583,344,0>占<1927,126,0>据
[35433,1273]<0,208,0>六<208,249,0>马<457,240,0>十<697,374,0>二<1071,202,0>兵
[37152,1063]<0,175,0>等<175,192,0>待<367,244,0>你<611,314,0>光<925,138,0>临
[39430,1972]<0,324,0>胡<324,229,0>琴<553,0,0> <553,227,0>诉<780,228,0>说<1008,208,0>英<1216,225,0>勇<1441,350,0>事<1791,181,0>迹
[41683,2515]<0,306,0>败<306,238,0>军<544,366,0>向<910,401,0>南<1311,201,0>远<1512,376,0>北<1888,433,0>方<2321,194,0>离
[45817,2068]<0,306,0>家<306,418,0>乡<724,0,0> <724,189,0>在<913,241,0>那<1154,208,0>美<1362,240,0>的<1602,312,0>远<1914,154,0>方
[48122,1152]<0,187,0>期<187,256,0>望<443,239,0>在<682,360,0>身<1042,110,0>上
[49636,1234]<0,186,0>梦<186,238,0>想<424,274,0>在<698,343,0>流<1041,193,0>浪
[52037,2017]<0,366,0>肩<366,202,0>上<568,0,0> <806,186,0>剩<992,264,0>下<1256,295,0>的<1551,287,0>能<1838,179,0>量
[54248,2577]<0,343,0>还<343,282,0>能<625,356,0>撑<981,428,0>到<1409,218,0>什<1627,390,0>么<2017,414,0>地<2431,146,0>方
[58171,4383]<0,647,0>等<647,797,0>待<1444,327,0>良<1771,384,0>人<2155,425,0>归<2580,499,0>来<3079,655,0>那<3734,429,0>一<4163,220,0>刻
[64370,2967]<0,736,0>眼<736,796,0>泪<1532,377,0>为<1909,433,0>你<2342,368,0>唱<2710,257,0>歌
[70215,2686]<0,192,0>在<192,280,0>我<472,352,0>离<824,273,0>你<1097,352,0>远<1449,374,0>去<1823,373,0>那<2196,292,0>一<2488,198,0>天
[73373,2808]<0,220,0>蓝<220,260,0>色<480,344,0>的<824,278,0>雨<1102,347,0>下<1449,420,0>在<1869,426,0>我<2295,345,0>眼<2640,168,0>前
[76620,3698]<0,169,0>骄<169,224,0>傲<393,304,0>的<697,241,0>泪<938,374,0>不<1312,440,0>敢<1752,370,0>弃<2122,415,0>守<2537,471,0>我<3008,434,0>眼<3442,256,0>睛
[82886,2672]<0,198,0>在<198,218,0>我<416,330,0>离<746,252,0>你<998,377,0>远<1375,424,0>去<1799,383,0>那<2182,331,0>一<2513,159,0>天
[85981,2807]<0,199,0>灰<199,258,0>色<457,352,0>的<809,255,0>梦<1064,350,0>睡<1414,498,0>在<1912,327,0>我<2239,382,0>身<2621,186,0>边
[89131,3644]<0,183,0>我<183,254,0>早<437,356,0>就<793,246,0>该<1039,352,0>习<1391,471,0>惯<1862,361,0>没<2223,439,0>有<2662,457,0>你<3119,303,0>的<3422,222,0>夜
[94363,1490]<0,319,0>勇<319,224,0>敢<543,376,0>的<919,399,0>面<1318,172,0>对
[102611,2089]<0,352,0>赤<352,368,0>壁<720,0,0> <720,207,0>烽<927,253,0>火<1180,189,0>连<1369,230,0>天<1599,328,0>战<1927,162,0>役
[104960,2702]<0,207,0>只<207,221,0>挂<428,256,0>掉<684,335,0>我<1019,210,0>们<1595,209,0>七<1804,228,0>万<2032,227,0>个<2259,304,0>兄<2563,139,0>弟
[108837,1983]<0,422,0>长<422,394,0>江<816,191,0>水<1007,233,0>面<1240,293,0>写<1533,301,0>日<1834,149,0>记
[111105,2185]<0,197,0>愿<197,203,0>你<400,248,0>也<648,349,0>能<997,348,0>看<1345,271,0>见<1616,369,0>涟<1985,200,0>漪
[115251,2048]<0,355,0>家<355,373,0>乡<728,0,0> <728,175,0>在<903,267,0>那<1170,173,0>美<1343,233,0>的<1576,319,0>远<1895,153,0>方
[117542,2759]<0,211,0>泪<211,230,0>水<441,231,0>背<672,383,0>着<1055,352,0>光<1407,0,0> <1598,195,0>安<1793,250,0>静<2043,220,0>而<2263,328,0>悲<2591,168,0>伤
[121568,1890]<0,349,0>肩<349,336,0>上<685,204,0>剩<889,260,0>下<1149,281,0>的<1430,304,0>能<1734,156,0>量
[123755,2479]<0,337,0>还<337,236,0>能<573,298,0>撑<871,410,0>到<1281,213,0>什<1494,401,0>么<1895,408,0>地<2303,176,0>方
[127522,4510]<0,762,0>等<762,782,0>待<1544,349,0>良<1893,418,0>人<2311,382,0>归<2693,365,0>来<3058,798,0>那<3856,411,0>一<4267,243,0>刻
[133793,2994]<0,798,0>眼<798,774,0>泪<1572,349,0>为<1921,405,0>你<2326,496,0>唱<2822,172,0>歌
[139703,2602]<0,200,0>在<200,213,0>我<413,352,0>离<765,267,0>你<1032,319,0>远<1351,443,0>去<1794,357,0>那<2151,258,0>一<2409,193,0>天
[142822,2851]<0,192,0>蓝<192,264,0>色<456,365,0>的<821,269,0>雨<1090,361,0>下<1451,419,0>在<1870,421,0>我<2291,342,0>眼<2633,218,0>前
[145993,3691]<0,193,0>骄<193,231,0>傲<424,328,0>的<752,262,0>泪<1014,384,0>不<1398,435,0>敢<1833,374,0>弃<2207,432,0>守<2639,398,0>我<3037,426,0>眼<3463,228,0>睛
[152290,2774]<0,188,0>在<188,251,0>我<439,360,0>离<799,269,0>你<1068,355,0>远<1423,431,0>去<1854,382,0>那<2236,346,0>一<2582,192,0>天
[155459,2803]<0,198,0>灰<198,269,0>色<467,336,0>的<803,251,0>梦<1054,344,0>睡<1398,513,0>在<1911,327,0>我<2238,368,0>身<2606,197,0>边
[158673,3670]<0,176,0>我<176,253,0>早<429,329,0>就<758,258,0>该<1016,386,0>习<1402,447,0>惯<1849,408,0>没<2257,345,0>有<2602,396,0>你<2998,395,0>的<3393,277,0>夜
[163790,1586]<0,355,0>勇<355,222,0>敢<577,313,0>的<890,455,0>面<1345,241,0>对
[165452,2985]<0,1518,0>我<1518,509,0>试<2027,340,0>着<2367,353,0>面<2720,265,0>对
[170084,1531]<0,353,0>灰<353,429,0>色<782,425,0>的<1207,324,0>夜
[173382,1369]<0,297,0>还<297,402,0>在<699,389,0>眼<1088,281,0>前
[178192,4440]<0,623,0>等<623,793,0>待<1416,380,0>良<1796,381,0>人<2177,412,0>归<2589,418,0>来<3007,752,0>那<3759,385,0>一<4144,296,0>刻
[184304,3039]<0,832,0>眼<832,801,0>泪<1633,351,0>为<1984,392,0>你<2376,423,0>唱<2799,240,0>歌
[190195,2806]<0,209,0>在<209,289,0>我<498,351,0>离<849,268,0>你<1117,324,0>远<1441,424,0>去<1865,352,0>那<2217,362,0>一<2579,227,0>天
[193313,2854]<0,211,0>蓝<211,282,0>色<493,307,0>的<800,289,0>雨<1089,367,0>下<1456,417,0>在<1873,391,0>我<2264,351,0>眼<2615,239,0>前
[196489,3721]<0,181,0>骄<181,276,0>傲<457,319,0>的<776,281,0>泪<1057,406,0>不<1463,441,0>敢<1904,449,0>弃<2353,329,0>守<2682,407,0>我<3089,410,0>眼<3499,222,0>睛
[202845,2780]<0,179,0>在<179,266,0>我<445,322,0>离<767,244,0>你<1011,335,0>远<1346,444,0>去<1790,367,0>那<2157,391,0>一<2548,232,0>天
[205967,2799]<0,230,0>灰<230,243,0>色<473,320,0>的<793,274,0>梦<1067,364,0>睡<1431,434,0>在<1865,358,0>我<2223,365,0>身<2588,211,0>边
[209178,3606]<0,187,0>我<187,261,0>早<448,314,0>就<762,244,0>该<1006,362,0>习<1368,481,0>惯<1849,402,0>没<2251,324,0>有<2575,376,0>你<2951,451,0>的<3402,204,0>夜
[214295,2092]<0,361,0>勇<361,277,0>敢<638,292,0>的<930,456,0>面<1386,706,0>对
)";

    std::string fileName = "D:/KuGou/KugouMusic/sanguo.krc";
    KRCDecoder decoder(fileName);
    std::string decodedData = decoder.getDecoded();
    K_LyricData lyricData = K_LyricParser::parseLyrics(decodedData);
    std::map<std::tuple<int, int, int, int>, std::pair<int, int>> timestampMap = K_LyricParser::buildTimestampMap(lyricData);

    int playbackPosition = 215000;

    // 记录开始时间
    auto start = std::chrono::high_resolution_clock::now();
    K_LyricParser::findLyricAtTime(lyricData, timestampMap, playbackPosition);
    // 记录结束时间
    auto end = std::chrono::high_resolution_clock::now();

    // 计算执行时间
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
    return 0;
}
*/
#endif //KLRICSPARSER_H
