import QtQuick

Item {
    id:root

    property alias demoMusicData: demoMusicData

    // 示例音乐数据
    ListModel {
        id: demoMusicData

        Component.onCompleted: {
            // 使用循环批量生成数据
            for (let i = 0; i < 10; i++) {
                demoMusicData.append({
                    index: i,
                    cover: "qrc:/assets/defaultAlbum.jpg",
                    title: "杜鹃",
                    artist: "Realzat / 加木",
                    album: "杜鹃",
                });
            }
        }
    }

    // 示例专辑数据
}
