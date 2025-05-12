import QtQuick 2.15
import QtQuick.Controls 2.15
import QtCharts
import FluentUI 1.0

FluWindow {
    visible: true
    width: 800
    height: 400
    title: "动态频谱图"
    // modality: Qt.ApplicationModal  // 阻塞整个应用程序的所有窗口[^9^][^10^]  即只允许这个窗口操作


    ChartView {
        id: chartView
        width: parent.width
        height: parent.height
        antialiasing: true
        legend.visible: false // 隐藏图例
        animationOptions: ChartView.SeriesAnimations // 启用动画效果

        BarSeries {
            id: barSeries
            axisX: BarCategoryAxis {
                categories: generateCategories(32)
                visible: false // 隐藏x坐标
            }
            axisY: ValueAxis {
                min: 0
                max: 100
                visible: false // 隐藏y坐标
            }

            BarSet {
                id: barSet
                label: "" // 设置为空字符串以隐藏标签
                values: generateValues(32) // 初始值
                color: "#FFFF3E84"
            }
        }

    }

    // JavaScript function to generate categories
    function generateCategories(count) {
        var categories = [];
        for (var i = 0; i < count; i++) {
            categories.push("Category " + (i + 1));
        }
        return categories;
    }

    // JavaScript function to generate values
    function generateValues(count) {
        // var values = [];
        // for (var i = 0; i < count; i++) {
        //     values.push(Math.random() * 100); // Generate random values between 0 and 100
        // }
        return analyzer.get_spectrumList();
    }

    // Timer to update the values periodically
    Timer {
        id: updateTimer
        interval: 1 // Update every 500 milliseconds
        repeat: true
        running: true
        onTriggered: {
            var newValues = generateValues(32);
            for (var i = 0; i < newValues.length; i++) {
                barSet.replace(i, newValues[i]); // 更新BarSet的值
            }
        }
    }
}