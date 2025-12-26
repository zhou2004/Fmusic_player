/* Service.js */
.import "./Request.js" as Request // 假设有一个 ApiClient 模块


/**
 * 业务层调用 (对应你想要的 js 逻辑)
 */
function getAllSongs(param) {
    return Request.getAllSongs(param).then(function(res) {
        if (res.code === 0) {
            console.log("Service: 获取成功");
            // 这里可以做一些数据预处理
            // 使用 JSON.stringify 格式化打印，缩进 4 个空格
            console.log("完整数据如下: \n" + JSON.stringify(res.data, null, 4));
            return res.data;
        } else {
            // 虽然 Promise resolved，但业务 code 不对，抛出错误进入 catch
            throw new Error(res.message);
        }
    }).catch(function(err) {
        console.log("Service: 出错了 - " + (err.message || "未知错误"));
        // 返回空数组，保证 UI 不崩
        return [];
    });
}