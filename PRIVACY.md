# Privacy Policy for FMusic

**Last updated:** December 29, 2025

This Privacy Policy describes how FMusic ("we", "us", or "our") collects, uses, and discloses information when you use our FMusic application (the "App") available on the Microsoft Store.

By using the App, you agree to the collection and use of information in accordance with this policy.

## 1. Information We Collect

FMusic is designed as a local-first music player. We prioritize your privacy and minimize data collection.

### 1.1. Local Music Files
The App requires access to your local file system (specifically your Music Library) to function.
* **Data Accessed:** Audio files (e.g., .mp3, .flac, .wav) and associated metadata (ID3 tags, cover art) stored on your device.
* **Usage:** This data is processed **locally** on your device solely to organize your library, display playlists, and perform audio playback.
* **Storage:** We do not upload your music files, metadata, or personal listening history to any external server.

### 1.2. Device Information
The App may utilize Windows System APIs (such as `Windows.System.Profile.SystemIdentification`) to generate a unique device identifier.
* **Purpose:** This identifier is used solely to associate local configuration files or playback states with your specific device.
* **Anonymity:** This identifier is a hashed value specific to this App publisher. It cannot be used to identify you personally, track your real-world identity, or track your location.

### 1.3. Internet Data (Lyrics and Metadata)
The App may connect to the internet to fetch additional media information not found in your local files, such as lyrics or high-quality album artwork.
* **Data Sent:** When fetching lyrics, the App may strictly send the **song title** and **artist name** to public music APIs.
* **No Tracking:** We do not store logs of these requests, nor do we build a user profile based on this data.

## 2. Third-Party Services

We do not sell, trade, or otherwise transfer your personally identifiable information to outside parties. However, the App is distributed via the Microsoft Store:

* **Microsoft Store:** Microsoft may collect usage data, crash reports, and installation statistics as part of the standard Windows telemetry system. This data collection is governed by [Microsoft's Privacy Statement](https://privacy.microsoft.com/en-us/privacystatement).

## 3. Data Security

The security of your data is important to us. Since FMusic operates primarily offline and stores databases (such as `playlists.db`) locally on your hard drive, your personal data remains in your possession. We recommend keeping your Windows operating system updated to ensure the security of your local files.

## 4. Children’s Privacy

FMusic does not knowingly collect or solicit personal data from anyone under the age of 13. The App is safe for all ages and focuses on playing content already owned by the user.

## 5. Changes to This Privacy Policy

We may update our Privacy Policy from time to time. We will notify you of any changes by posting the new Privacy Policy on this page (or the repository hosting this file). You are advised to review this Privacy Policy periodically for any changes.

## 6. Contact Us

If you have any questions about this Privacy Policy or the App, please contact us at:

* **Developer:** Feel Liao
* **Email:** feel2027@outlook.com
* **GitHub Repository:** https://github.com/zhou2004/Fmusic_player