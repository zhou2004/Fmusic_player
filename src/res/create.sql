create table if not exists PlaylistType
(
    tag_id   varchar(36)  not null comment '歌单标签的唯一标识'
    primary key,
    tag_name varchar(100) not null comment '歌单标签名'
    );

create table if not exists art
(
    art_name  varchar(100) not null comment '艺术家名称'
    primary key,
    album     varchar(100) null comment '代表作品专辑',
    cover_url varchar(200) null comment '艺术家封面URL'
    )
    comment '艺术家信息表';

create table if not exists classify
(
    type_name varchar(100) not null comment '分类名称(如流行、摇滚)'
    primary key,
    introduce varchar(100) null comment '分类简介',
    cover_url varchar(200) null comment '分类封面图URL'
    )
    comment '音乐分类信息表';

create table if not exists music
(
    music_id      int auto_increment comment '音乐ID'
    primary key,
    music_name    varchar(200)                not null comment '音乐名称',
    art_name      varchar(100)                not null comment '艺术家名称',
    cover_url     varchar(200)                null comment '音乐封面URL',
    lyric_url     varchar(200)                null comment '歌词文件URL',
    type_name     varchar(100)                null comment '音乐分类',
    play_count    bigint unsigned default '0' null comment '播放次数',
    collect_count bigint unsigned default '0' null comment '收藏次数',
    play_url      varchar(200)                not null comment '播放地址',
    album         varchar(100)                null comment '专辑名',
    duration      int                         null comment '播放时长',
    constraint fk_music_art
    foreign key (art_name) references art (art_name)
    on update cascade on delete cascade,
    constraint fk_music_classify
    foreign key (type_name) references classify (type_name)
    on delete set null
    )
    comment '音乐信息表';

create index idx_art_name
    on music (art_name);

create index idx_music_name
    on music (music_name);

create index idx_type_name
    on music (type_name);

create table if not exists user
(
    user_id         bigint auto_increment comment '用户ID(雪花算法生成)'
    primary key,
    user_password   varchar(64)                       not null comment '登录密码(SHA256加密存储)',
    user_avatar_url varchar(200)                      null comment '用户头像URL',
    email           varchar(50)                       null comment '邮箱(唯一)',
    phone           char(11)                          null comment '手机号(唯一)',
    nickname        varchar(50)                       not null comment '昵称(唯一，默认=用户ID)',
    gender          enum ('1', '-1', '0') default '0' null comment '性别(1:男 -1:女 0:未设置)',
    age             int unsigned                      null comment '年龄(0-120)',
    constraint uni_email
    unique (email),
    constraint uni_nickname
    unique (nickname),
    constraint uni_phone
    unique (phone)
    )
    comment '用户基础信息表';

create table if not exists Playlist
(
    playlist_id   int auto_increment
    primary key,
    playlist_name varchar(100) not null comment '歌单名',
    user_id       bigint       null comment '链接用户id',
    create_time   date         null comment '歌单创建时间',
    description   text         null,
    cover_url     varchar(100) not null comment '歌单专辑图片',
    tag_id        varchar(36)  null comment '歌单标签id-用于分类',
    constraint Playlist_ibfk_1
    foreign key (user_id) references user (user_id),
    constraint Playlist_ibfk_2
    foreign key (tag_id) references PlaylistType (tag_id)
    );

create index tag_id
    on Playlist (tag_id);

create index user_id
    on Playlist (user_id);

create table if not exists PlaylistDetails
(
    playlist_id int not null,
    music_id    int not null,
    primary key (playlist_id, music_id),
    constraint PlaylistDetails_ibfk_1
    foreign key (playlist_id) references Playlist (playlist_id),
    constraint PlaylistDetails_ibfk_2
    foreign key (music_id) references music (music_id)
    );

create index music_id
    on PlaylistDetails (music_id);

create table if not exists UserCollections
(
    collection_id   int auto_increment comment '收藏记录ID'
    primary key,
    user_id         bigint                              not null comment '用户ID',
    music_id        int                                 not null comment '音乐ID',
    item_type       varchar(20)                         null comment '收藏类型(歌单/单曲)',
    collection_time timestamp default CURRENT_TIMESTAMP null comment '收藏时间',
    collect_name    varchar(20)                         null,
    constraint unique_user_music
    unique (user_id, music_id),
    constraint UserCollections_ibfk_1
    foreign key (user_id) references user (user_id),
    constraint fk_collection_music
    foreign key (music_id) references music (music_id)
    on update cascade on delete cascade,
    constraint fk_collection_user
    foreign key (user_id) references user (user_id)
    on delete cascade
    )
    comment '用户收藏记录表';

create index idx_collection_time
    on UserCollections (collection_time);

create index idx_user_collection
    on UserCollections (user_id, music_id);

create table if not exists UserHistory
(
    history_id     int auto_increment comment '历史记录ID'
    primary key,
    user_id        bigint                              not null comment '用户ID',
    music_id       int                                 not null comment '音乐ID',
    item_type      varchar(20)                         null comment '操作类型(播放/下载)',
    operation_time timestamp default CURRENT_TIMESTAMP null comment '操作时间',
    operation_type varchar(20)                         null comment '操作类型',
    constraint fk_history_music
    foreign key (music_id) references music (music_id)
    on update cascade on delete cascade,
    constraint fk_history_user
    foreign key (user_id) references user (user_id)
    on delete cascade
    )
    comment '用户行为历史表';

create index idx_operation_time
    on UserHistory (operation_time);

create index idx_user_history
    on UserHistory (user_id asc, operation_time desc);

create table if not exists comments
(
    comments_id   int auto_increment comment '评论ID'
    primary key,
    comments_r    varchar(1000)                       not null comment '评论内容',
    user_id       bigint                              not null comment '用户ID',
    music_id      int                                 not null comment '音乐ID',
    comments_time timestamp default CURRENT_TIMESTAMP null comment '评论时间',
    constraint fk_comments_music
    foreign key (music_id) references music (music_id)
    on update cascade on delete cascade,
    constraint fk_comments_user
    foreign key (user_id) references user (user_id)
    on delete cascade
    )
    comment '用户评论表';

create fulltext index ft_comments
    on comments (comments_r);

create index idx_comments_time
    on comments (comments_time);

create definer = admin@`%` trigger trg_user_nickname_default
    before insert
    on user
    for each row
BEGIN
    -- 当昵称为空时自动设置为用户ID字符串
    IF (NEW.nickname IS NULL OR TRIM(NEW.nickname) = '') THEN
        SET NEW.nickname = CAST(NEW.user_id AS CHAR(20));
END IF;
END;

create or replace definer = admin@`%` view v_music_statistics as
select `m`.`music_id`                        AS `music_id`,
       `m`.`music_name`                      AS `music_name`,
       count(distinct `uc`.`user_id`)        AS `collect_users`,
       count(distinct `uh`.`user_id`)        AS `play_users`,
       sum((`uh`.`operation_type` = 'play')) AS `total_plays`
from ((`fmusic`.`music` `m` left join `fmusic`.`UserCollections` `uc`
       on ((`m`.`music_id` = `uc`.`music_id`))) left join `fmusic`.`UserHistory` `uh`
      on ((`m`.`music_id` = `uh`.`music_id`)))
group by `m`.`music_id`;

-- comment on column v_music_statistics.music_id not supported: 音乐ID

-- comment on column v_music_statistics.music_name not supported: 音乐名称

create or replace definer = admin@`%` view v_user_activity as
select `u`.`user_id`                        AS `user_id`,
       `u`.`nickname`                       AS `nickname`,
       count(distinct `uc`.`collection_id`) AS `collections`,
       count(distinct `uh`.`history_id`)    AS `plays`,
       count(distinct `c`.`comments_id`)    AS `comments`
from (((`fmusic`.`user` `u` left join `fmusic`.`UserCollections` `uc`
        on ((`u`.`user_id` = `uc`.`user_id`))) left join `fmusic`.`UserHistory` `uh`
       on ((`u`.`user_id` = `uh`.`user_id`))) left join `fmusic`.`comments` `c` on ((`u`.`user_id` = `c`.`user_id`)))
group by `u`.`user_id`;

-- comment on column v_user_activity.user_id not supported: 用户ID(雪花算法生成)

-- comment on column v_user_activity.nickname not supported: 昵称(唯一，默认=用户ID)

create
definer = admin@`%` procedure InsertMusicToPlaylist(IN playlist_id int)
BEGIN
    DECLARE i INT DEFAULT 101;

    WHILE i <= 200 DO
            INSERT INTO fmusic.PlaylistDetails (playlist_id, music_id)
SELECT playlist_id, i
FROM DUAL
WHERE EXISTS (SELECT 1 FROM fmusic.music WHERE music_id = i);

SET i = i + 1;
END WHILE;
END;

create
definer = admin@`%` procedure InsertPlaylistTypes()
BEGIN
    DECLARE tag_id INT DEFAULT 0;
    DECLARE tag_name VARCHAR(100);
    DECLARE done INT DEFAULT FALSE;
    DECLARE cur CURSOR FOR SELECT tag_name FROM (SELECT '流行' AS tag_name UNION ALL
                                                 SELECT '摇滚' UNION ALL
                                                 SELECT '电子' UNION ALL
                                                 SELECT '古典' UNION ALL
                                                 SELECT '爵士' UNION ALL
                                                 SELECT '嘻哈' UNION ALL
                                                 SELECT '民谣' UNION ALL
                                                 SELECT '乡村' UNION ALL
                                                 SELECT '金属' UNION ALL
                                                 SELECT '蓝调' UNION ALL
                                                 SELECT '世界音乐' UNION ALL
                                                 SELECT '新世纪' UNION ALL
                                                 SELECT '灵魂' UNION ALL
                                                 SELECT '雷鬼' UNION ALL
                                                 SELECT '朋克' UNION ALL
                                                 SELECT '放克' UNION ALL
                                                 SELECT '拉丁' UNION ALL
                                                 SELECT '凯尔特' UNION ALL
                                                 SELECT '印度' UNION ALL
                                                 SELECT '非洲' UNION ALL
                                                 SELECT '中东' UNION ALL
                                                 SELECT '中国' UNION ALL
                                                 SELECT '日本' UNION ALL
                                                 SELECT '韩国' UNION ALL
                                                 SELECT '古典跨界' UNION ALL
                                                 SELECT '电影原声' UNION ALL
                                                 SELECT '儿童' UNION ALL
                                                 SELECT '圣诞' UNION ALL
                                                 SELECT '冥想' UNION ALL
                                                 SELECT '运动' UNION ALL
                                                 SELECT '派对' UNION ALL
                                                 SELECT '工作' UNION ALL
                                                 SELECT '学习' UNION ALL
                                                 SELECT '睡眠') AS tag_list;
DECLARE CONTINUE HANDLER FOR NOT FOUND SET done = TRUE;

OPEN cur;

read_loop: LOOP
        FETCH cur INTO tag_name;
        IF done THEN
            LEAVE read_loop;
END IF;

INSERT INTO fmusic.PlaylistType (tag_id, tag_name)
VALUES (tag_id, tag_name)
    ON DUPLICATE KEY UPDATE tag_name = tag_name;

SET tag_id = tag_id + 1;
END LOOP;

CLOSE cur;
END;

create
definer = admin@`%` function encrypt_password(pwd varchar(20)) returns varchar(64) deterministic
BEGIN
RETURN SHA2(CONCAT(pwd, 'salt_string'), 256); -- 加盐哈希
END;

