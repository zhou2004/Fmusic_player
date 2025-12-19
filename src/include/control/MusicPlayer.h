// MusicPlayer.h
/**
* @file MusicPlayer.h
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
#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaMetaData>
#include <QBuffer>
#include <QVariantList>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QStandardPaths>
#include <QRandomGenerator>

#include <QVector>
#include <QString>

#include "interface.h" // 引入 TrackModel / AudioState 等模型
#include "../db/LocalTrackTable.h"   // 新增：本地歌曲表操作类

// MusicPlayer: 对应前端 AudioStore 的标准音乐播放控制类
// 负责：
//  - 维护播放列表（trackList）和当前播放索引（currentSongIndex）
//  - 控制播放 / 暂停 / 上一首 / 下一首 / 跳转进度 / 调整音量
//  - 暴露必要的 Q_INVOKABLE 接口给 QML 使用
//  - 内部使用 QMediaPlayer + QAudioOutput，不对外暴露裸指针，避免内存泄漏
class MusicPlayer : public QObject {
    Q_OBJECT

    // ==================== QML 可绑定属性 ====================
    Q_PROPERTY(int currentSongIndex READ currentSongIndex WRITE setCurrentSongIndex NOTIFY currentSongIndexChanged)
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY volumeChanged)
    // 播放模式：0=顺序播放 1=列表循环 2=单曲循环 3=随机播放
    Q_PROPERTY(int playMode READ playMode WRITE setPlayMode NOTIFY playModeChanged)

public:
    // ======================= 构造 =======================
    explicit MusicPlayer(QObject *parent = nullptr);
    // ======================= 析构 =======================
    ~MusicPlayer() override;

    // ============= 播放控制接口（供 QML / C++ 调用） =============
    /**
     *
     * @brief 播放当前 trackList[currentSongIndex]
     *
     * @return 没有返回值。
     */
    Q_INVOKABLE void play();

    /**
     *
     * @brief 根据索引播放指定歌曲（会自动更新 currentSongIndex）
     *
     * @return 没有返回值。
     */
    Q_INVOKABLE void playIndex(int index);

    /**
     *
     * @brief 播放指定 URL（不进入播放列表）
     *
     * @return 没有返回值。
     */
    Q_INVOKABLE void playUrl(const QString &url);

    /**
     *
     * @brief 暂停
     *
     * @return 没有返回值。
     */
    Q_INVOKABLE void pause();

    /**
     *
     * @brief 停止
     *
     * @return 没有返回值。
     */
    Q_INVOKABLE void stop();

    /**
     *
     * @brief 上一首
     *
     * @return 没有返回值。
     */
    Q_INVOKABLE void previous();

    /**
     *
     * @brief 下一首
     *
     * @return 没有返回值。
     */
    Q_INVOKABLE void next();

    /**
     *
     * @brief 跳转到指定毫秒位置
     *
     * @param positionMs 毫秒位置
     * @return 没有返回值。
     */
    Q_INVOKABLE void seek(qint64 positionMs);

    // ============= 播放列表 / 当前页歌曲管理 =============

    /**
     *
     * @brief 使用完整的 TrackModel 列表替换当前播放列表
     *
     *@param tracks 新的播放列表
     * @return 没有返回值。
     */
    Q_INVOKABLE void setTrackList(const QVariantList &tracks);

    /**
     *
     * @brief 向播放列表末尾追加一首歌或多首歌
     *
     *@param tracks 新的播放列表
     * @return 没有返回值。
     */
    Q_INVOKABLE void addTracks(const QVariantList &tracks);

    /**
     *
     * @brief 删除指定 id 的歌曲
     *
     *@param id 歌曲 ID(索引)
     * @return 没有返回值。
     */
    Q_INVOKABLE void deleteTrack(const QString &id);

    /**
     *
     * @brief 设置当前页面的歌曲列表（不一定等于播放列表），用于 UI 显示
     *
     *@param songs 新的当前页面歌曲列表
     * @return 没有返回值。
     */
    Q_INVOKABLE void setCurrentPageSongs(const QVariantList &songs);

    /**
     *
     * @brief 读取当前页面歌曲列表（返回 QJsonArray 方便 QML 使用）
     *
     * @return 歌曲列表。
     */
    Q_INVOKABLE QJsonArray currentPageSongsJson() const;

    // ============= 状态读取接口 =============
    /**
     *
     * @brief 返回当前播放索引
     *
     * @return index播放索引。
     */
    int currentSongIndex() const { return m_state.currentSongIndex; }

    /**
     *
     * @brief 设置当前播放索引
     *
     *@param index 播放索引
     * @return 没有返回值。
     */
    void setCurrentSongIndex(int index);

    /**
     *
     * @brief 获取当前音量（0-100）
     *
     * @return 音量值。
     */
    int volume() const { return m_state.volume; }
    /**
     *
     * @brief 设置当前音量（0-100）
     *
     *@param volume 音量值
     * @return 无
     */
    void setVolume(int volume);

    /**
     *
     * @brief 获取当前播放歌曲的元信息（title/artist/album/duration/cover 等）
     *
     * @return 当前歌曲元信息。
     */
    Q_INVOKABLE QJsonObject currentTrackInfo() const;

    //@TODO: 本地音乐接口，直接和 MusicPlayer 集成在一起，不单独做 LocalMusicPlayer 类
    // ============= 本地音乐相关接口（直接集成在 MusicPlayer 中） =============

    /**
     *
     * @brief 打开系统目录选择对话框，扫描本地音乐文件并填充到本地列表
     *
     * @return 无返回值。
     */
    Q_INVOKABLE void openLocalMusicFolder();

    //@TODO: 本地音乐扫描，需要实现多线程扫描，避免阻塞 UI，还需要使用 QMediaMetaData提取元数据
    /**
     *
     * @brief 扫描指定目录下的音乐文件（本地路径或 file:// URL 均可），不弹对话框
     * 对于歌曲里面的元数据（标题、艺术家、专辑、封面等），使用 QMediaMetaData 读取(还没有提取实现)。
     * 将扫描到的音乐文件信息存储到本地 SQLite 数据库中，供本地音乐播放使用。
     *
     *@param path 目录路径
     * @return 无返回值。
     */
    Q_INVOKABLE void scanLocalDirectory(const QString &path);

    //@TODO: 此函数不需要单独写，将云音乐和本地音乐列表合并到一个播放列表中即可，可删除
    /**
     *
     * @brief 提供给 QML 的本地音乐列表模型，元素结构与 trackModel 一致
     *
     * @return 播放队列。
     */
    Q_INVOKABLE QVariantList localTracks() const { return m_localTracks; }

    /**
     *
     * @brief 获取本地音乐总数
     *
     * @return 音乐总数。
     */
    Q_INVOKABLE int localTracksCount() const;

    /**
     *
     * @brief 分页获取本地音乐列表
     *
     *@param offset 偏移量
     *@param limit 数量限制
     * @return 音乐列表。
     */
    Q_INVOKABLE QVariantList localTracksPage(int offset, int limit) const;

    /**
     *
     * @brief 获取当前播放模式
     *
     * @return 播放模式。
     */
    int playMode() const { return m_playMode; }

    /**
     *
     * @brief 设置当前播放模式
     *
     *@param mode 播放模式
     * @return 无
     */
    void setPlayMode(int mode);

    // ==================== 信号接口-事件与回调 ====================
signals:
    /**
    *
    * @brief 当前播放索引变化，通知 QML 更新显示
    *
    *@param index 播放索引
    * @return 无
    */
    void currentSongIndexChanged(int index);

    /**
    *
    * @brief 当前播放音量变化，通知 QML 更新显示
    *
    *@param volume 更新的音量值
    * @return 无
    */
    void volumeChanged(int volume);

    /**
    *
    * @brief 播放状态变化，通知 QML 更新显示
    *
    *@param volume 更新的播放状态
    * @return 无
    */
    void playingChanged(bool playing);

    //@TODO: 进度变化和曲目信息变化信号,这两个信号需要在内部回调中触发，是实时变化的
    /**
    *
    * @brief 播放进度变化（毫秒），通知 QML 更新显示
    *
    *@param positionMs 播放进度
    * @return 无
    */
    void positionChanged(qint64 positionMs);

    /**
    *
    * @brief 当前播放曲目信息变化，通知 QML 更新显示
    *
    *@param info 曲目信息
    * @return 无
    */
    void currentTrackInfoChanged(const QJsonObject &info);

    /**
    *
    * @brief 当前播放模式变化，通知 QML 更新显示
    *
    *@param mode 播放模式
    * @return 无
    */
    void playModeChanged(int mode);

    //==================== 本地音乐相关信号 ====================
    //@TODO: 本地音乐列表变化信号，在扫描完成后触发
    /**
    *
    * @brief 本地播放队列变化，通知 QML 更新显示
    * 此信号在本地音乐扫描完成后触发，在其他情况一般不触发
    *
    * @return 无
    */
    void localTracksChanged();


private slots:
    // QMediaPlayer 内部回调：进度变化
    void handlePositionChanged(qint64 position);

    // QMediaPlayer 内部回调：播放状态变化
    void handlePlaybackStateChanged(QMediaPlayer::PlaybackState state);

    // 播放器状态改变时调用，用于通知 QML 数据已更新，调用信号：metaDataChanged
    void mediaStatusChanged(QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::LoadedMedia) {

        }
    }

private:
    // 实际的 Qt 播放引擎
    QMediaPlayer *m_player = nullptr;
    QAudioOutput *m_audioOutput = nullptr;

    /**
     * @brief AudioState，存储播放列表、当前索引、音量等信息，是播放器的核心状态
     * 此变量只会实例化一次，所有播放相关的状态都存储在这里，方便管理和传递
     * 不可以直接暴露给 QML 使用，必须通过访问接口访问
    */
    AudioState m_state;

    // 当前页面歌曲列表（仅用于 UI 显示，不一定参与播放队列）
    QVector<TrackModel> m_currentPageSongs;

    //@TODO: 此变量已弃用，可以删除
    // 本地扫描到的音乐列表（不一定等于当前播放列表）
    QVariantList m_localTracks;

    // 播放模式：0 顺序, 1 列表循环, 2 单曲循环, 3 随机
    int m_playMode = 1;

    // 本地歌曲表操作对象（用于持久化到 SQLite）
    LocalTrackTable m_localTrackTable;

    // 帮助函数：根据当前索引加载对应的 media
    void loadCurrentTrack();

    // 从 TrackModel 构造 JSON，方便 QML 使用
    static QJsonObject trackToJson(const TrackModel &track);
};

#endif // MUSICPLAYER_H

