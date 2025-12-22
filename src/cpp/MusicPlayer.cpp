/**
 * @file MusicPlayer.cpp
 *
 * @brief 音乐播放器核心实现,包含播放控制、播放列表管理和本地音乐扫描功能。
 *
 * 该类使用 Qt Multimedia 模块实现音频播放，支持播放控制接口、音乐列表管理和信号等处理。
 * 实现了本地音乐文件扫描功能，并将扫描结果存储到 SQLite 数据库中，专门用于本地音乐播放。
 * 实现了播放模式（顺序、循环、单曲循环、随机）和音量控制等功能。
 * 实现了与 QML 的交互接口，方便在 QML 界面中使用播放器功能。
 * 实现了当前播放曲目信息的访问接口，供界面显示使用。
 * 实现了播放进度和状态变化的信号通知，供界面实时更新使用。
 * 实现了播放列表的增删改查接口，支持动态管理播放列表。
 *
 * @author zhou2004
 * @date 2025-12-18
 */

#include "MusicPlayer.h"
#include "KLyricsParser.h"
#include <QVariant>
#include <QUrl>
#include <QFileDialog>
#include <QDirIterator>
#include <QFileInfo>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>
#include <qimagewriter.h>
#include <QThread>

MusicPlayer::MusicPlayer(QObject *parent)
    : QObject(parent)
{
    // 初始化默认 AudioState，里面存储当前播放队列，当前歌曲索引，声音等信息
    m_state.currentSongIndex = 0;
    m_state.volume = 50;       // 0~100
    m_state.quality = "exhigh";

    // 初始化底层播放器
    m_audioOutput = new QAudioOutput(this);
    m_player = new QMediaPlayer(this);
    m_player->setAudioOutput(m_audioOutput);

    // 默认音量
    m_audioOutput->setVolume(m_state.volume / 100.0f);

    // 连接内部信号到外部 signal，供 QML 绑定
    connect(m_player, &QMediaPlayer::positionChanged,
            this, &MusicPlayer::handlePositionChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged,
            this, &MusicPlayer::handlePlaybackStateChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &MusicPlayer::mediaStatusChanged);

    // \[新增] 初始化扫描 watcher，用于监听后台扫描任务结束
    m_scanWatcher = new QFutureWatcher<QVector<TrackModel>>(this);
    // 当 future 执行完毕时，在主线程调用 onScanFinished
    connect(m_scanWatcher, &QFutureWatcher<QVector<TrackModel>>::finished,
            this, &MusicPlayer::onScanFinished);
}

MusicPlayer::~MusicPlayer() = default;

// ======================= 播放控制接口 =======================

void MusicPlayer::play()
{
    // 如果当前列表为空，直接返回
    if (m_state.trackList.isEmpty())
        return;

    // 如果当前索引越界，则重置到 0
    if (m_state.currentSongIndex < 0
        || m_state.currentSongIndex >= m_state.trackList.size()) {
        m_state.currentSongIndex = 0;
        emit currentSongIndexChanged(m_state.currentSongIndex);
    }

    // 加载并播放当前曲目
    if (!m_player->hasAudio()) loadCurrentTrack();
    m_player->play();
}

void MusicPlayer::playIndex(int index)
{
    if (index < 0 || index >= m_state.trackList.size())
        return;

    setCurrentSongIndex(index);
    loadCurrentTrack();
    m_player->play();
}

void MusicPlayer::playUrl(const QString &url)
{
    if (url.isEmpty())
        return;

    m_player->setSource(QUrl(url));
    m_audioOutput->setVolume(m_state.volume / 100.0f);
    m_player->play();
}

void MusicPlayer::pause()
{
    m_player->pause();
}

void MusicPlayer::stop()
{
    m_player->stop();
}

void MusicPlayer::previous()
{
    if (m_state.trackList.isEmpty())
        return;

    // 只计算一次上一曲索引，并通过 playIndex 触发播放
    int newIndex = m_state.currentSongIndex - 1;
    if (newIndex < 0) {
        // 列表循环：跳到最后一首
        newIndex = m_state.trackList.size() - 1;
    }
    playIndex(newIndex);
}

void MusicPlayer::next()
{
    if (m_state.trackList.isEmpty())
        return;

    // 只计算一次下一曲索引，并通过 playIndex 触发播放
    int newIndex = (m_state.currentSongIndex + 1) % m_state.trackList.size();
    playIndex(newIndex);
}

void MusicPlayer::seek(qint64 positionMs)
{
    if (!m_player)
        return;
    m_player->setPosition(positionMs);
}

// ======================= 播放列表 / 当前页歌曲管理 =======================

void MusicPlayer::setTrackList(const QVariantList &tracks)
{
    // 清空现有列表
    m_state.trackList.clear();

    // 重新添加新列表
    for (const QVariant &v : tracks) {
        const QVariantMap map = v.toMap();
        TrackModel t;
        t.id        = map.value("id").toString();
        t.title     = map.value("title").toString();
        t.artist    = map.value("artist").toString();
        t.album     = map.value("album").toString();
        t.cover     = map.value("cover").toString();
        t.url       = map.value("url").toString();
        t.duration  = map.value("duration").toInt();
        t.likeStatus = map.value("likeStatus").toInt();
        t.lyrics    = map.value("lyrics").toString();
        m_state.trackList.append(t);
    }

    // 重置索引策略，如果新列表非空则从 0 开始，否则设为 -1
    if (!m_state.trackList.isEmpty()) {
        m_state.currentSongIndex = 0;
        emit currentSongIndexChanged(0);
    } else {
        m_state.currentSongIndex = -1;
        emit currentSongIndexChanged(-1);
    }
}

QVector<TrackModel> MusicPlayer::getTrackList() {
    return m_state.trackList;
}

//@TODO: 目前是通过遍历检查重复添加，效率较低，后续可以改为通过哈希表检查
void MusicPlayer::addTracks(const QVariantList &tracks)
{
    // 检查重复添加，避免重复 id
    for (const QVariant &v : tracks) {
        const QVariantMap map = v.toMap();
        const QString id = map.value("id").toString();

        // 检查是否已存在相同 id
        bool exists = false;
        for (const TrackModel &t : std::as_const(m_state.trackList)) {
            if (t.id == id) {
                exists = true;
                break;
            }
        }

        // 关键：跳过已存在的曲目，避免重复添加
        if (exists)
            continue;

        // 添加新曲目
        TrackModel t;
        t.id        = id;
        t.title     = map.value("title").toString();
        t.artist    = map.value("artist").toString();
        t.album     = map.value("album").toString();
        t.cover     = map.value("cover").toString();
        t.url       = map.value("url").toString();
        t.duration  = map.value("duration").toInt();
        t.likeStatus = map.value("likeStatus").toInt();
        t.lyrics    = map.value("lyrics").toString();
        m_state.trackList.append(t);
    }

    // 如果之前是空列表，则从 0 开始
    if (m_state.currentSongIndex < 0 && !m_state.trackList.isEmpty()) {
        m_state.currentSongIndex = 0;
        emit currentSongIndexChanged(0);
    }
}

//@TODO: 目前是通过遍历查找并删除，效率较低，后续可以改为通过索引删除
void MusicPlayer::deleteTrack(const QString &id)
{
    // 查找并删除指定 id 的曲目，不是通过索引，删除效率较低
    for (int i = 0; i < m_state.trackList.size(); ++i) {
        if (m_state.trackList[i].id == id) {
            m_state.trackList.removeAt(i);

            // 调整 currentSongIndex
            if (m_state.currentSongIndex >= m_state.trackList.size()) {
                m_state.currentSongIndex = m_state.trackList.size() - 1;
            }
            emit currentSongIndexChanged(m_state.currentSongIndex);
            break;
        }
    }
}

void MusicPlayer::setCurrentPageSongs(const QVariantList &songs)
{
    // 清空现有列表
    m_currentPageSongs.clear();
    // 重新添加新列表
    for (const QVariant &v : songs) {
        const QVariantMap map = v.toMap();
        TrackModel t;
        t.id        = map.value("id").toString();
        t.title     = map.value("title").toString();
        t.artist    = map.value("artist").toString();
        t.album     = map.value("album").toString();
        t.cover     = map.value("cover").toString();
        t.url       = map.value("url").toString();
        t.duration  = map.value("duration").toInt();
        t.likeStatus = map.value("likeStatus").toInt();
        t.lyrics    = map.value("lyrics").toString();
        m_currentPageSongs.append(t);
    }
}

QJsonArray MusicPlayer::currentPageSongsJson() const
{
    QJsonArray arr;
    for (const TrackModel &t : m_currentPageSongs) {
        arr.append(trackToJson(t));
    }
    return arr;
}

// ======================= 状态访问接口 =======================

void MusicPlayer::setCurrentSongIndex(int index)
{
    if (m_state.currentSongIndex == index)
        return;

    m_state.currentSongIndex = index;
    emit currentSongIndexChanged(index);
}

void MusicPlayer::setVolume(int volume)
{
    if (volume < 0)
        volume = 0;
    if (volume > 100)
        volume = 100;

    if (m_state.volume == volume)
        return;

    m_state.volume = volume;
    emit volumeChanged(volume);

    if (m_audioOutput)
        m_audioOutput->setVolume(static_cast<float>(volume) / 100.0f);
}

QJsonObject MusicPlayer::currentTrackInfo() const
{
    if (m_state.currentSongIndex < 0
        || m_state.currentSongIndex >= m_state.trackList.size()) {
        return QJsonObject();
    }
    return trackToJson(m_state.trackList[m_state.currentSongIndex]);
}


void MusicPlayer::setPlayMode(int mode)
{
    if (m_playMode == mode)
        return;
    m_playMode = mode;
    emit playModeChanged(mode);
}

// ======================= 本地音乐目录扫描 =======================

void MusicPlayer::openLocalMusicFolder()
{
    const QString startPath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    QString dirPath = QFileDialog::getExistingDirectory(nullptr,
                                                        tr("Select Music Folder"),
                                                        startPath);
    if (dirPath.isEmpty())
        return;

    scanLocalDirectory(dirPath);
}

// TODO: 这里的扫描和元数据提取使用了 QtConcurrent，在后台线程执行，避免阻塞 UI,已完成本地音乐扫描功能。
void MusicPlayer::scanLocalDirectory(const QString &path)
{
    // 清空旧的 QML 本地列表缓存（你现在主要用 SQLite，这里只是保持一致）
    m_localTracks.clear();

    // 如果上一次扫描还在进行，直接忽略本次请求（也可以选择 cancel）
    if (m_scanWatcher && m_scanWatcher->isRunning()) {
        qWarning() << "MusicPlayer::scanLocalDirectory scan already running, skip.";
        return;
    }

    // 支持 file:/// 开头的 URL
    QString localPath = path;
    if (localPath.startsWith("file:")) {
        QUrl url(localPath);
        localPath = url.toLocalFile();
    }

    QDir dir(localPath);
    if (!dir.exists()) {
        qWarning() << "MusicPlayer::scanLocalDirectory directory not exist:" << localPath;
        emit localTracksChanged();
        return;
    }

    // \[核心] 使用 QtConcurrent::run 在后台线程执行扫描和元数据提取
    // 注意：这里使用 lambda 捕获 localPath，避免直接访问 this 中的 QObject 成员
    auto future = QtConcurrent::run([localPath]() -> QVector<TrackModel> {
        QVector<TrackModel> scannedTracks;

        // 递归遍历目录下所有支持的音频文件
        QDirIterator it(localPath,
                        QStringList() << "*.mp3" << "*.flac" << "*.wav"
                                      << "*.m4a" << "*.ogg" << "*.aac",
                        QDir::Files,
                        QDirIterator::Subdirectories);

        while (it.hasNext()) {
            const QString filePath = it.next();
            QFileInfo info(filePath);

            TrackModel t;
            // 这里使用绝对路径作为 id，确保在 SQLite 中唯一且不重复
            t.id  = filePath;
            t.url = QUrl::fromLocalFile(filePath).toString();

            // ----------- 使用 QMediaPlayer+QMediaMetaData 读取元数据（在工作线程） -----------
            QMediaPlayer metaPlayer;
            QAudioOutput audioOutput;
            metaPlayer.setAudioOutput(&audioOutput);
            metaPlayer.setSource(QUrl::fromLocalFile(filePath));

            // 等待元数据加载完成
                QEventLoop loop;
                connect(&metaPlayer, &QMediaPlayer::mediaStatusChanged, &loop, [&loop](QMediaPlayer::MediaStatus status) {
                    if (status == QMediaPlayer::LoadedMedia) {
                        loop.quit();
                    }
                });
                loop.exec();

            QMediaMetaData md = metaPlayer.metaData();

            // 标题：优先用元数据 Title，否则退回到文件名（不带扩展名）
            t.title = md.value(QMediaMetaData::Title).toString();
            if (t.title.isEmpty()) {
                t.title = info.completeBaseName();
            }

            // 艺术家：常用的是 ContributingArtist，某些文件可能只写了 Author 等
            t.artist = md.stringValue(QMediaMetaData::ContributingArtist);
            if (t.artist.isEmpty()) {
                t.artist = QStringLiteral("Unknown Artist");
            }

            // 专辑
            t.album = md.stringValue(QMediaMetaData::AlbumTitle);
            if (t.album.isEmpty()) {
                t.album = QStringLiteral("Unknown Album");
            }

            // 时长：QMediaMetaData::Duration 一般是毫秒
            QVariant durVar = md.value(QMediaMetaData::Duration);
            // 你的 TrackModel 中 duration 是 int（通常单位秒），这里转成秒存
            t.duration = durVar.isValid() ? durVar.toInt() / 1000 : 0;

            // 封面：这里简单使用默认图，也可以用 CoverArtImage 保存到缓存再给一个本地路径
            // ... 在 lambda 里替换封面处理代码：
            QImage albumArt = md.value(QMediaMetaData::ThumbnailImage).value<QImage>();

            if (!albumArt.isNull()) {
                // 1. 确定一个缓存目录，比如: <临时目录>/music_covers
                const QString cacheRoot = QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                                          + "/music_covers";
                QDir().mkpath(cacheRoot);

                // 2. 以文件 md5 或完整路径做文件名，避免重复（这里简单用 baseName）
                const QString baseName = info.completeBaseName();
                const QString coverPath = cacheRoot + "/" + baseName + ".jpg";

                // 3. 写成 jpg 文件
                QImageWriter writer(coverPath, "jpg");
                if (writer.write(albumArt)) {
                    // 4. 保存为 file:// URL 或绝对路径，QML 都可以加载
                    t.cover = QUrl::fromLocalFile(coverPath).toString();
                } else {
                    // 写失败则用默认封面
                    t.cover = QStringLiteral("qrc:/assets/defaultAlbum.jpg");
                }
            } else {
                // 没有嵌入封面，使用默认封面
                t.cover = QStringLiteral("qrc:/assets/defaultAlbum.jpg");
            }

            // 其它扩展字段
            t.likeStatus = 0;
            // t.lyrics = extract_Lyrics(filePath);
            // 假设 t.lyrics 是 QString 类型
            t.lyrics = QString::fromUtf8(u8R"([0,17600]<0,1173,0>三<1173,1173,0>国<2346,1174,0>恋<3520,1173,0> <4693,1173,0>(<5866,1174,0>女<7040,1173,0>版<8213,1173,0>)<9386,1174,0> <10560,1173,0>-<11733,1173,0> <12906,1174,0>黎<14080,1173,0>林<15253,1173,0>添<16426,1174,0>娇
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
)");

            // qWarning().noquote() << t.lyrics.toUtf8().constData();

            scannedTracks.append(t);
        }

        return scannedTracks;
    });

    // 让 watcher 监听这个 future，结束后会触发 onScanFinished（在主线程执行）
    m_scanWatcher->setFuture(future);
}

// 扫描完成后的主线程回调：在这里写 SQLite，并通知 QML
void MusicPlayer::onScanFinished()
{
    if (!m_scanWatcher)
        return;

    // 从 future 取出后台线程返回的扫描结果
    m_scannedTracks = m_scanWatcher->result();

    // 将扫描结果写入 SQLite（通过 LocalTrackTable 封装）
    if (!m_localTrackTable.ensureTable()) {
        qWarning() << "MusicPlayer: ensure local_tracks table failed";
    } else {
        if (!m_localTrackTable.clearAll()) {
            qWarning() << "MusicPlayer: clear local_tracks failed";
        }
        if (!m_localTrackTable.insertOrReplaceBatch(m_scannedTracks)) {
            qWarning() << "MusicPlayer: insert scanned tracks to sqlite failed";
        }
    }

    // 通知 QML：本地音乐列表已经更新（QML 通过 localTracksPage 从 SQLite 读取）
    emit localTracksChanged();

    qDebug() << "MusicPlayer scanned local tracks count (sqlite):" << m_scannedTracks.size();
}

int MusicPlayer::localTracksCount() const
{
    // 直接从 SQLite 的 local_tracks 表统计总数
    return m_localTrackTable.count();
}

QVariantList MusicPlayer::localTracksPage(int offset, int limit) const
{
    // 安全检查留给表类或这里简单防御
    if (offset < 0 || limit <= 0)
        return QVariantList();

    // 直接从 SQLite 的 local_tracks 表分页查询
    return m_localTrackTable.page(offset, limit);
}

QVariantList MusicPlayer::queryTracksPaged(
        int offset,
        int limit,
        const QString &artist,
        const QString &album,
        const QString &title,
        const QString &orderBy,
        bool orderAsc
    ) const {
    // 安全检查留给表类或这里简单防御
    if (offset < 0 || limit <= 0)
        return QVariantList();
    return m_localTrackTable.queryTracksPaged(
        offset, limit, artist, album, title, orderBy, orderAsc);
}


// ======================= 内部回调 & 辅助函数实现 =======================

void MusicPlayer::handlePositionChanged(qint64 position)
{
    // 将底层 QMediaPlayer 的 positionChanged 直接转发给 QML
    emit positionChanged(position);
}

void MusicPlayer::handlePlaybackStateChanged(QMediaPlayer::PlaybackState state)
{
    const bool playing = (state == QMediaPlayer::PlayingState);
    emit playingChanged(playing);

    // 只在自然停止（播放结束）时根据播放模式切歌，避免和手动点击 next/previous 冲突
    if (state == QMediaPlayer::StoppedState && m_player && m_player->position() > 0) {
        if (m_playMode == 2) {
            // 单曲循环
            playIndex(m_state.currentSongIndex);
        } else if (m_playMode == 1 || m_playMode == 0) {
            // 列表循环或顺序播放：自动下一首
            next();
        } else if (m_playMode == 3) {
            // 随机播放
            if (!m_state.trackList.isEmpty()) {
                int newIndex = QRandomGenerator::global()->bounded(m_state.trackList.size());
                playIndex(newIndex);
            }
        }
    }
}

void MusicPlayer::loadCurrentTrack()
{
    if (m_state.currentSongIndex < 0 || m_state.currentSongIndex >= m_state.trackList.size()) {
        return;
    }

    const TrackModel &t = m_state.trackList[m_state.currentSongIndex];

    // 这里假设 url 字段已经是完整的可播放 URL（本地或网络）
    m_player->setSource(QUrl(t.url));

    // 播放前同步设置音量
    m_audioOutput->setVolume(static_cast<float>(m_state.volume) / 100.0f);

    // 通知当前曲目信息变化（附带一个 durationMs 字段，给进度条总时长使用）
    QJsonObject info = trackToJson(t);
    // info["duration"] = m_player->metaData().value(QMediaMetaData::Duration).toInt();
    // qDebug () << "Title:" << m_player->metaData().value(QMediaMetaData::AlbumTitle).toString();
    // qDebug () << "duration ms:"<< info["duration"].toInt();
    // 如果 trackModel 中没有真实时长，这里可以先用 duration 字段的秒数估算
    // if (!info.contains("durationMs")) {
    //     int sec = m_player->metaData().value(QMediaMetaData::Duration).toInt();
    //     // int sec = info.value("duration").toInt();
    //     info["durationMs"] = static_cast<qint64>(sec) * 1000;
    // }
    emit currentTrackInfoChanged(info);
}

// @TODO: 该函数可以提取到 Utils 里复用
// 辅助函数：将 TrackModel 转为 QJsonObject，后续可以写到Utils里复用
QJsonObject MusicPlayer::trackToJson(const TrackModel &t)
{
    QJsonObject obj;
    obj["id"]        = t.id;
    obj["title"]     = t.title;
    obj["artist"]    = t.artist;
    obj["album"]     = t.album;
    obj["cover"]     = t.cover;
    obj["url"]       = t.url;
    obj["duration"]  = t.duration;
    obj["likeStatus"] = t.likeStatus;
    obj["lyrics"]    = t.lyrics;
    return obj;
}
