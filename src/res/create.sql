--添加Playlist歌单数据操作：
-- 假设 user 表和 PlaylistType 表中已经有以下数据
-- 需要有对应user来能创建歌单，这样就能确定歌单是由哪个用户创建的
INSERT INTO user (user_id, username, password, email, created_at) VALUES
    (1, 'user1', 'password1', 'user1@example.com', '2023-01-01');


-- 插入数据到 Playlist 表
INSERT INTO Playlist (playlist_name, user_id, create_time, description, cover_url, tag_id) VALUES
    ( 'My Playlist', 1, '2023-01-01', 'This is my playlist', 'http://example.com/playlist.jpg', "1");




-- 删除Playlist歌单数据操作：

-- 如果 PlaylistDetails 表中有引用 playlist_id = 1 的数据，需要先删除这些数据：
sql
复制
-- 删除 PlaylistDetails 表中引用 playlist_id = 1 的数据
DELETE FROM PlaylistDetails WHERE playlist_id = 1;

-- 删除 Playlist 表中的数据
DELETE FROM Playlist WHERE playlist_id = 1;