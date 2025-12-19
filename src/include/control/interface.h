//
// Created by 周俊杰 on 2025/12/14.
//

#ifndef INTERFACE_H
#define INTERFACE_H

#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>

// 对应 TS: PlaylistSong
struct PlaylistSong {
    int songId = 0;
    QString songName;
    QString artistName;
    QString album;
    QString duration;
    QString coverUrl;   // null 用空字符串表示
    QString audioUrl;
    int likeStatus = 0;
    QString releaseTime; // null 用空字符串表示

    static PlaylistSong fromJson(const QJsonObject &obj) {
        PlaylistSong s;
        s.songId      = obj.value("songId").toInt();
        s.songName    = obj.value("songName").toString();
        s.artistName  = obj.value("artistName").toString();
        s.album       = obj.value("album").toString();
        s.duration    = obj.value("duration").toString();
        s.coverUrl    = obj.value("coverUrl").toString();
        s.audioUrl    = obj.value("audioUrl").toString();
        s.likeStatus  = obj.value("likeStatus").toInt();
        s.releaseTime = obj.value("releaseTime").toString();
        return s;
    }
};

// 对应 TS: PlaylistComment / Comment（结构相同）
struct PlaylistComment {
    int commentId = 0;
    QString username;
    QString userAvatar; // null 用空字符串
    QString content;
    QString createTime;
    int likeCount = 0;

    static PlaylistComment fromJson(const QJsonObject &obj) {
        PlaylistComment c;
        c.commentId  = obj.value("commentId").toInt();
        c.username   = obj.value("username").toString();
        c.userAvatar = obj.value("userAvatar").toString();
        c.content    = obj.value("content").toString();
        c.createTime = obj.value("createTime").toString();
        c.likeCount  = obj.value("likeCount").toInt();
        return c;
    }
};

using Comment = PlaylistComment;

// 对应 TS: PlaylistDetail
struct PlaylistDetail {
    int playlistId = 0;
    QString title;
    QString coverUrl;   // null -> 空串
    QString introduction;
    QVector<PlaylistSong> songs;
    int likeStatus = 0;
    QVector<PlaylistComment> comments;
    bool isCollected = false;

    static PlaylistDetail fromJson(const QJsonObject &obj) {
        PlaylistDetail p;
        p.playlistId    = obj.value("playlistId").toInt();
        p.title         = obj.value("title").toString();
        p.coverUrl      = obj.value("coverUrl").toString();
        p.introduction  = obj.value("introduction").toString();
        p.likeStatus    = obj.value("likeStatus").toInt();
        p.isCollected   = obj.value("isCollected").toBool();

        // songs 数组
        QJsonArray songsArr = obj.value("songs").toArray();
        p.songs.reserve(songsArr.size());
        for (const QJsonValue &v : songsArr) {
            if (v.isObject()) {
                p.songs.append(PlaylistSong::fromJson(v.toObject()));
            }
        }

        // comments 数组
        QJsonArray commentsArr = obj.value("comments").toArray();
        p.comments.reserve(commentsArr.size());
        for (const QJsonValue &v : commentsArr) {
            if (v.isObject()) {
                p.comments.append(PlaylistComment::fromJson(v.toObject()));
            }
        }

        return p;
    }
};

// 对应 TS: Song
struct Song {
    int songId = 0;
    QString songName;
    QString artistName;
    QString album;
    QString lyric;     // null -> 空串
    QString duration;
    QString coverUrl;
    QString audioUrl;
    int likeStatus = 0;
    QString releaseTime;

    static Song fromJson(const QJsonObject &obj) {
        Song s;
        s.songId      = obj.value("songId").toInt();
        s.songName    = obj.value("songName").toString();
        s.artistName  = obj.value("artistName").toString();
        s.album       = obj.value("album").toString();
        s.lyric       = obj.value("lyric").toString();
        s.duration    = obj.value("duration").toString();
        s.coverUrl    = obj.value("coverUrl").toString();
        s.audioUrl    = obj.value("audioUrl").toString();
        s.likeStatus  = obj.value("likeStatus").toInt();
        s.releaseTime = obj.value("releaseTime").toString();
        return s;
    }
};

// 对应 TS: SongDetail
struct SongDetail {
    int songId = 0;
    QString songName;
    QString artistName;
    QString album;
    QString lyric;      // null -> 空串
    QString duration;
    QString coverUrl;
    QString audioUrl;
    QString releaseTime;
    int likeStatus = 0; // TS: boolean | null，这里用 int 表示（0/1，或 -1 表示 null）
    QVector<Comment> comments;

    static SongDetail fromJson(const QJsonObject &obj) {
        SongDetail s;
        s.songId      = obj.value("songId").toInt();
        s.songName    = obj.value("songName").toString();
        s.artistName  = obj.value("artistName").toString();
        s.album       = obj.value("album").toString();
        s.lyric       = obj.value("lyric").toString();
        s.duration    = obj.value("duration").toString();
        s.coverUrl    = obj.value("coverUrl").toString();
        s.audioUrl    = obj.value("audioUrl").toString();
        s.releaseTime = obj.value("releaseTime").toString();

        // likeStatus: boolean | null
//        const QJsonValue vLike = obj.value("likeStatus");
//        if (vLike.isBool()) {
//            s.likeStatus = vLike.toBool() ? 1 : 0;
//        } else if (vLike.isNull()) {
//            s.likeStatus = -1; // 表示 null
//        } else {
//            s.likeStatus = obj.value("likeStatus").toInt();
//        }
//
//        // comments 数组
//        QJsonArray commentsArr = obj.value("comments").toArray();
//        s.comments.reserve(commentsArr.size());
//        for (const QJsonValue &v : commentsArr) {
//            if (v.isObject()) {
//                s.comments.append(Comment::fromJson(v.toObject()));
//            }
//        }

        return s;
    }
};

// ==================== 用户及播放状态 / 设置 / 主题 ====================

// 对应 TS: userModel
struct UserModel {
    QString avatarUrl;  // 头像
    QString username;   // 用户名
    int userId = 0;     // 用户 id
    QString token;      // 用户 token，可为空

    static UserModel fromJson(const QJsonObject &obj) {
        UserModel u;
        u.avatarUrl = obj.value("avatarUrl").toString();
        u.username  = obj.value("username").toString();
        u.userId    = obj.value("userId").toInt();
        u.token     = obj.value("token").toString();
        return u;
    }
};

// 对应 TS: UserState
struct UserState {
    UserModel userInfo; // 这里不做 Partial，使用默认值表示“未设置”
    bool isLoggedIn = false;
};

// 对应 TS: trackModel
struct TrackModel {
    QString id;        // 歌曲 id
    QString title;     // 歌曲名
    QString artist;    // 艺术家
    QString album;     // 专辑
    QString cover;     // 封面
    QString url;       // 音频地址
    int duration = 0;  // 时长（秒或毫秒，按你约定）
    int likeStatus = 0;
    QString lyrics;    // 歌词，可为空

    static TrackModel fromSong(const Song &song) {
        TrackModel t;
        t.id        = QString::number(song.songId);
        t.title     = song.songName;
        t.artist    = song.artistName;
        t.album     = song.album;
        t.cover     = song.coverUrl;
        t.url       = song.audioUrl;
        t.duration  = song.duration.toDouble(); // 如果后端 duration 是字符串，这里简单转 double
        t.likeStatus = song.likeStatus;
        t.lyrics    = song.lyric;
        return t;
    }
};

// 对应 TS: AudioState
struct AudioState {
    QVector<TrackModel> trackList;  // 歌曲缓存
    int currentSongIndex = -1;      // 当前播放歌曲索引，-1 表示无
    QVector<TrackModel> currentPageSongs; // 当前页面的歌曲列表
    int volume = 100;               // 音量 0-100
    QString quality;                // 音质（例如 "high" / "standard" 等）
};

// 对应 TS: MenuState
struct MenuState {
    QString menuIndex;  // 当前菜单索引
};

// 对应 TS: SettingState
struct SettingState {
    bool isDrawerCover = false;      // 是否覆盖抽屉
    bool isOriginalParsed = false;   // 是否解析原文
    bool isRomaParsed = false;       // 是否解析罗马音
    bool isTranslatedParsed = false; // 是否解析翻译
    QString language;                // 当前系统语言，为空表示跟随系统
};

// 对应 TS: ThemeState
struct ThemeState {
    bool isDark = false;  // 是否暗黑模式
    QString primary;      // 主题色，例如 "#409EFF"
};

#endif // INTERFACE_H
