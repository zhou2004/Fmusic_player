// Request.js
/**
 * 将 信号/槽 模式封装为 Promise
 */

function login(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.loginSuccess.disconnect(onSuccess);
            apiClient.loginError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.loginSuccess.disconnect(onSuccess);
            apiClient.loginError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.loginSuccess.disconnect(onSuccess);
        apiClient.loginError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.login(data);
    });
}

function logout() {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.logoutSuccess.disconnect(onSuccess);
            apiClient.logoutError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.logoutSuccess.disconnect(onSuccess);
            apiClient.logoutError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.logoutSuccess.disconnect(onSuccess);
        apiClient.logoutError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.logout();
    });
}

function sendEmailCode(email) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.sendEmailCodeSuccess.disconnect(onSuccess);
            apiClient.sendEmailCodeError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.sendEmailCodeSuccess.disconnect(onSuccess);
            apiClient.sendEmailCodeError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.sendEmailCodeSuccess.disconnect(onSuccess);
        apiClient.sendEmailCodeError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.sendEmailCode(email);
    });
}

function registerUser(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.registerUserSuccess.disconnect(onSuccess);
            apiClient.registerUserError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.registerUserSuccess.disconnect(onSuccess);
            apiClient.registerUserError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.registerUserSuccess.disconnect(onSuccess);
        apiClient.registerUserError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.registerUser(data);
    });
}

function resetPassword(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.resetPasswordSuccess.disconnect(onSuccess);
            apiClient.resetPasswordError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.resetPasswordSuccess.disconnect(onSuccess);
            apiClient.resetPasswordError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.resetPasswordSuccess.disconnect(onSuccess);
        apiClient.resetPasswordError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.resetPassword(data);
    });
}

function getUserInfo() {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getUserInfoSuccess.disconnect(onSuccess);
            apiClient.getUserInfoError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getUserInfoSuccess.disconnect(onSuccess);
            apiClient.getUserInfoError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getUserInfoSuccess.disconnect(onSuccess);
        apiClient.getUserInfoError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getUserInfo();
    });
}

function updateUserInfo(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.updateUserInfoSuccess.disconnect(onSuccess);
            apiClient.updateUserInfoError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.updateUserInfoSuccess.disconnect(onSuccess);
            apiClient.updateUserInfoError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.updateUserInfoSuccess.disconnect(onSuccess);
        apiClient.updateUserInfoError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.updateUserInfo(data);
    });
}

function deleteUser() {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.deleteUserSuccess.disconnect(onSuccess);
            apiClient.deleteUserError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.deleteUserSuccess.disconnect(onSuccess);
            apiClient.deleteUserError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.deleteUserSuccess.disconnect(onSuccess);
        apiClient.deleteUserError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.deleteUser();
    });
}

function getBanner() {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getBannerSuccess.disconnect(onSuccess);
            apiClient.getBannerError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getBannerSuccess.disconnect(onSuccess);
            apiClient.getBannerError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getBannerSuccess.disconnect(onSuccess);
        apiClient.getBannerError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getBanner();
    });
}

function getRecommendedPlaylists() {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getRecommendedPlaylistsSuccess.disconnect(onSuccess);
            apiClient.getRecommendedPlaylistsError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getRecommendedPlaylistsSuccess.disconnect(onSuccess);
            apiClient.getRecommendedPlaylistsError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getRecommendedPlaylistsSuccess.disconnect(onSuccess);
        apiClient.getRecommendedPlaylistsError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getRecommendedPlaylists();
    });
}

function getRecommendedSongs() {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getRecommendedSongsSuccess.disconnect(onSuccess);
            apiClient.getRecommendedSongsError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getRecommendedSongsSuccess.disconnect(onSuccess);
            apiClient.getRecommendedSongsError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getRecommendedSongsSuccess.disconnect(onSuccess);
        apiClient.getRecommendedSongsError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getRecommendedSongs();
    });
}

function getAllSongs(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getAllSongsSuccess.disconnect(onSuccess);
            apiClient.getAllSongsError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getAllSongsSuccess.disconnect(onSuccess);
            apiClient.getAllSongsError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getAllSongsSuccess.disconnect(onSuccess);
        apiClient.getAllSongsError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getAllSongs(data);
    });
}

function getAllVipSongs(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getAllVipSongsSuccess.disconnect(onSuccess);
            apiClient.getAllVipSongsError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getAllVipSongsSuccess.disconnect(onSuccess);
            apiClient.getAllVipSongsError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getAllVipSongsSuccess.disconnect(onSuccess);
        apiClient.getAllVipSongsError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getAllVipSongs(data);
    });
}

function getSongDetail(id) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getSongDetailSuccess.disconnect(onSuccess);
            apiClient.getSongDetailError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getSongDetailSuccess.disconnect(onSuccess);
            apiClient.getSongDetailError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getSongDetailSuccess.disconnect(onSuccess);
        apiClient.getSongDetailError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getSongDetail(id);
    });
}

function getAllArtists(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getAllArtistsSuccess.disconnect(onSuccess);
            apiClient.getAllArtistsError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getAllArtistsSuccess.disconnect(onSuccess);
            apiClient.getAllArtistsError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getAllArtistsSuccess.disconnect(onSuccess);
        apiClient.getAllArtistsError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getAllArtists(data);
    });
}

function getArtistDetail(id) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getArtistDetailSuccess.disconnect(onSuccess);
            apiClient.getArtistDetailError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getArtistDetailSuccess.disconnect(onSuccess);
            apiClient.getArtistDetailError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getArtistDetailSuccess.disconnect(onSuccess);
        apiClient.getArtistDetailError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getArtistDetail(id);
    });
}

function getAllPlaylists(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getAllPlaylistsSuccess.disconnect(onSuccess);
            apiClient.getAllPlaylistsError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getAllPlaylistsSuccess.disconnect(onSuccess);
            apiClient.getAllPlaylistsError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getAllPlaylistsSuccess.disconnect(onSuccess);
        apiClient.getAllPlaylistsError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getAllPlaylists(data);
    });
}

function getPlaylistDetail(id) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getPlaylistDetailSuccess.disconnect(onSuccess);
            apiClient.getPlaylistDetailError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getPlaylistDetailSuccess.disconnect(onSuccess);
            apiClient.getPlaylistDetailError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getPlaylistDetailSuccess.disconnect(onSuccess);
        apiClient.getPlaylistDetailError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getPlaylistDetail(id);
    });
}

function getFavoriteSongs(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getFavoriteSongsSuccess.disconnect(onSuccess);
            apiClient.getFavoriteSongsError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getFavoriteSongsSuccess.disconnect(onSuccess);
            apiClient.getFavoriteSongsError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getFavoriteSongsSuccess.disconnect(onSuccess);
        apiClient.getFavoriteSongsError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getFavoriteSongs(data);
    });
}

function collectSong(songId) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.collectSongSuccess.disconnect(onSuccess);
            apiClient.collectSongError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.collectSongSuccess.disconnect(onSuccess);
            apiClient.collectSongError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.collectSongSuccess.disconnect(onSuccess);
        apiClient.collectSongError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.collectSong(songId);
    });
}

function cancelCollectSong(songId) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.cancelCollectSongSuccess.disconnect(onSuccess);
            apiClient.cancelCollectSongError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.cancelCollectSongSuccess.disconnect(onSuccess);
            apiClient.cancelCollectSongError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.cancelCollectSongSuccess.disconnect(onSuccess);
        apiClient.cancelCollectSongError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.cancelCollectSong(songId);
    });
}

function getFavoritePlaylists(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.getFavoritePlaylistsSuccess.disconnect(onSuccess);
            apiClient.getFavoritePlaylistsError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.getFavoritePlaylistsSuccess.disconnect(onSuccess);
            apiClient.getFavoritePlaylistsError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.getFavoritePlaylistsSuccess.disconnect(onSuccess);
        apiClient.getFavoritePlaylistsError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.getFavoritePlaylists(data);
    });
}

function collectPlaylist(playlistId) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.collectPlaylistSuccess.disconnect(onSuccess);
            apiClient.collectPlaylistError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.collectPlaylistSuccess.disconnect(onSuccess);
            apiClient.collectPlaylistError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.collectPlaylistSuccess.disconnect(onSuccess);
        apiClient.collectPlaylistError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.collectPlaylist(playlistId);
    });
}

function cancelCollectPlaylist(playlistId) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.cancelCollectPlaylistSuccess.disconnect(onSuccess);
            apiClient.cancelCollectPlaylistError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.cancelCollectPlaylistSuccess.disconnect(onSuccess);
            apiClient.cancelCollectPlaylistError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.cancelCollectPlaylistSuccess.disconnect(onSuccess);
        apiClient.cancelCollectPlaylistError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.cancelCollectPlaylist(playlistId);
    });
}

function addSongComment(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.addSongCommentSuccess.disconnect(onSuccess);
            apiClient.addSongCommentError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.addSongCommentSuccess.disconnect(onSuccess);
            apiClient.addSongCommentError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.addSongCommentSuccess.disconnect(onSuccess);
        apiClient.addSongCommentError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.addSongComment(data);
    });
}

function addPlaylistComment(data) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.addPlaylistCommentSuccess.disconnect(onSuccess);
            apiClient.addPlaylistCommentError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.addPlaylistCommentSuccess.disconnect(onSuccess);
            apiClient.addPlaylistCommentError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.addPlaylistCommentSuccess.disconnect(onSuccess);
        apiClient.addPlaylistCommentError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.addPlaylistComment(data);
    });
}

function likeComment(commentId) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.likeCommentSuccess.disconnect(onSuccess);
            apiClient.likeCommentError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.likeCommentSuccess.disconnect(onSuccess);
            apiClient.likeCommentError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.likeCommentSuccess.disconnect(onSuccess);
        apiClient.likeCommentError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.likeComment(commentId);
    });
}

function cancelLikeComment(commentId) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.cancelLikeCommentSuccess.disconnect(onSuccess);
            apiClient.cancelLikeCommentError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.cancelLikeCommentSuccess.disconnect(onSuccess);
            apiClient.cancelLikeCommentError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.cancelLikeCommentSuccess.disconnect(onSuccess);
        apiClient.cancelLikeCommentError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.cancelLikeComment(commentId);
    });
}

function deleteComment(commentId) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.deleteCommentSuccess.disconnect(onSuccess);
            apiClient.deleteCommentError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.deleteCommentSuccess.disconnect(onSuccess);
            apiClient.deleteCommentError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.deleteCommentSuccess.disconnect(onSuccess);
        apiClient.deleteCommentError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.deleteComment(commentId);
    });
}

function addFeedback(content) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.addFeedbackSuccess.disconnect(onSuccess);
            apiClient.addFeedbackError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.addFeedbackSuccess.disconnect(onSuccess);
            apiClient.addFeedbackError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.addFeedbackSuccess.disconnect(onSuccess);
        apiClient.addFeedbackError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.addFeedback(content);
    });
}

function createOrder(orderNo, userId, planId) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.createOrderSuccess.disconnect(onSuccess);
            apiClient.createOrderError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.createOrderSuccess.disconnect(onSuccess);
            apiClient.createOrderError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.createOrderSuccess.disconnect(onSuccess);
        apiClient.createOrderError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.createOrder(orderNo, userId, planId);
    });
}

function payOrder(orderNo) {
    return new Promise(function(resolve, reject) {

        // --- 1. 定义回调函数 ---

        // 成功回调
        var onSuccess = function(response) {
            // 【关键】必须断开连接，否则下次请求会触发多次回调
            apiClient.payOrderSuccess.disconnect(onSuccess);
            apiClient.payOrderError.disconnect(onError);

            // Promise 成功
            resolve(response);
        };

        // 失败回调
        var onError = function(msg, code) {
            // 【关键】必须断开连接
            apiClient.payOrderSuccess.disconnect(onSuccess);
            apiClient.payOrderError.disconnect(onError);

            // Promise 失败
            reject({ message: msg, code: code });
        };

        // --- 2. 连接 C++ 信号 ---
        // 在 Qt 6 / QML 中，信号是一个对象，拥有 connect/disconnect 方法
        apiClient.payOrderSuccess.disconnect(onSuccess);
        apiClient.payOrderError.disconnect(onError);

        // --- 3. 触发 C++ 请求 ---
        apiClient.payOrder(orderNo);
    });
}
