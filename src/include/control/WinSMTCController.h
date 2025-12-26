#pragma once

#include <QObject>
#include <QJsonObject>

#ifdef ENABLE_SMTC
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.h>
#include <winrt/Windows.Storage.Streams.h>

class WinSMTCController : public QObject {
    Q_OBJECT
public:
    explicit WinSMTCController(QObject* parent = nullptr);
    ~WinSMTCController();

    Q_INVOKABLE void updateMetadata(const QJsonObject &info);
    Q_INVOKABLE void setPlaybackState(bool playing);
    Q_INVOKABLE void setPosition(qint64 ms);

signals:
    void playRequested();
    void pauseRequested();
    void nextRequested();
    void previousRequested();
    void seekRequested(qint64 ms);

    // Optional: emits when metadata is observed from current global session
    void metadataObserved(const QJsonObject &info);

private:
    // Platform-specific initialization helpers
    void initWinRT();
    void shutdownWinRT();

    // Lazy init: bind SMTC to the app's top-level HWND using GetForWindow(HWND)
    bool ensureSmtcReady();

    // Throttle state for timeline updates
    qint64 m_lastUpdateTime = 0;
    qint64 m_lastSentMs = 0;

    // WinRT objects for SMTC
    winrt::Windows::Media::SystemMediaTransportControls m_smtc{ nullptr };
    winrt::Windows::Media::SystemMediaTransportControlsDisplayUpdater m_updater{ nullptr };

    // Event tokens for clean unregistration
    winrt::event_token m_btnToken{};
    winrt::event_token m_seekToken{};

    // Store last metadata provided by application
    QJsonObject m_lastMetadata;
};
#endif