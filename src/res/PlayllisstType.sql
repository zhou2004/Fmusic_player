-- 插入音乐类型数据
SET @tag_id = 0;

INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '流行' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '摇滚' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '电子' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '古典' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '爵士' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '嘻哈' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '民谣' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '乡村' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '金属' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '蓝调' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '世界音乐' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '新世纪' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '灵魂' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '雷鬼' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '朋克' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '放克' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '拉丁' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '凯尔特' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '印度' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '非洲' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '中东' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '中国' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '日本' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '韩国' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '古典跨界' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '电影原声' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '儿童' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '圣诞' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '冥想' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '运动' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '派对' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '工作' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '学习' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);

SET @tag_id = @tag_id + 1;
INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
SELECT @tag_id, '睡眠' FROM DUAL WHERE NOT EXISTS (SELECT 1 FROM fmusic.PlaylistType WHERE tag_id = @tag_id);