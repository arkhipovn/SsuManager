import QtQuick 2.12
import QCC.DataView.ChartViewExtension 1.0
import QtCharts 2.3
import Theme 1.0
import SSU 1.0

XYChart {
    id: measurementChartView

    chartLegend: ChartLegend {
        spacing: 0
    }

    margins { bottom: markerLabel.height + Theme.pixelSize; }

    MarkerValueLabel {
        id: markerLabel
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom; margins: Theme.indent; }
        wrapMode: MarkerValueLabel.WordWrap
        horizontalAlignment: Qt.AlignRight
        chartView: measurementChartView
    }

    property var maskSeries: null
    property var model: null
    onModelChanged: {
        updateChartView()
        updateChartViewData()
    }

    Component.onDestruction: {
        removeMaskSeries()
        removeSeriesList()
        if(currentAxisY != null) {
            currentAxisY.destroy()
            currentAxisY = null
        }
    }

    property int dataType: model == null ? SSU.Invalid
                                         : model.dataType

    property var currentAxisY: null

    /// createAxis

    Connections {
        target: measurementChartView.model

        onMaskChanged: {
            if(measurementChartView.model.useMask) {
                if(maskSeries == null) {
                    // Нужно создать если еще нет
                    createMaskSeries()
                }
                else {
                    // Уже есть. Достаточно только имя поменять
                    maskSeries.name = measurementChartView.model.mask.name
                }

                // Обновим данные и оси
                updateMaskSeriesData()
                updateDomain()
            }
            else measurementChartView.removeMaskSeries()
        }

        onRowsRemoved: {
            //@N вот это переделать бы.. Зачем все удалять? Пока что на скорую руку сделаю так
            removeSeriesList()          // Удалим старые
            createSeriesList()          // Создадим заново список
            updateSeriesListData()      // Обновим данные
            updateDomain()
        }

        onRowsInserted: {
            //@N вот это переделать бы.. Зачем все удалять? Пока что на скорую руку сделаю так
            removeSeriesList()          // Удалим старые
            createSeriesList()          // Создадим заново список
            updateSeriesListData()      // Обновим данные
            updateDomain()
        }

        onDataChanged: {
            if(!roles.includes(0))  //@N Qt.DisplayRole..
                return
            // Обновились данные
            // Определим нужно ли поменять ось Y?

            switch(dataType) {
            case SSU.Rf:
                if(currentAxisY == null || !(currentAxisY instanceof ValueAxis)) {
                    // нужно все удалить что было раньше
                    updateChartView()
                }
                break
            case SSU.Mtie:
            case SSU.Tdev:
            default:
                if(currentAxisY == null || currentAxisY instanceof ValueAxis) {
                    // нужно все удалить что было раньше
                    updateChartView()
                }
                break
            }

            updateChartViewData()
        }
    }

    Component {
        id: valueAxisComponet

        ValueAxis {
            labelFormat: "%g"
            min: 1; max: 10
            minorTickCount: 2
        }
    }

    Component {
        id: logValueAxisComponet

        LogValueAxis {
            labelFormat: "%g"
            min: 1; max: 10
            minorTickCount: -1
        }
    }

    LogValueAxis {
        id: valueAxisX
        labelFormat: "%g"
        min: 1; max: 10
    }

    Component {
        id: lowerSeriesComponent
        LineSeries {}
    }
    // Удаление всех серий. Создание оси Y и всех серий с этой axisY
    function updateChartView() {
        if(currentAxisY != null) {
            removeMaskSeries()
            removeSeriesList()
            currentAxisY.destroy()
        }

        currentAxisY = createAxis()
        createSeriesList()          // Создадим заново список
        if(model.useMask)   {
            createMaskSeries()
        }
    }

    function updateChartViewData() {
        updateSeriesListData()
        updateMaskSeriesData()
        updateDomain()
    }

    function createAxis() {
        switch(dataType) {
        case SSU.Rf:
            return valueAxisComponet.createObject(measurementChartView)
        case SSU.Mtie:
        case SSU.Tdev:
        default:
            return logValueAxisComponet.createObject(measurementChartView)
        }
    }

    function createSeriesList() {
        if(model == null)
            return
        for(let i = 0; i < model.rowCount(); ++i) {
            var series = measurementChartView.createSeries(ChartView.SeriesTypeLine,
                                                           model.headerData(i, Qt.Vertical),
                                                           valueAxisX, currentAxisY)
            series.visibleChanged.connect(measurementChartView.updateDomain)
            series.useOpenGL = false
            series.pointsVisible = true
        }
    }

    function createMaskSeries() {
        if(model == null)
            return
        maskSeries = measurementChartView.createSeries(ChartView.SeriesTypeArea,
                                                       qsTr("Mask") + ": " + model.mask.name,
                                                       valueAxisX, currentAxisY)
        maskSeries.visibleChanged.connect(measurementChartView.updateDomain)
        maskSeries.color = "red"
        maskSeries.useOpenGL = false
        maskSeries.opacity = 0.2
        maskSeries.borderColor = "red"
        if(dataType == SSU.Rf) {
            maskSeries.lowerSeries = lowerSeriesComponent.createObject(maskSeries)      // Не знаю как правильно..
        }
    }

    function removeSeriesList() {
        if(maskSeries == null) {
            // Просто удалим все
            measurementChartView.removeAllSeries()
        }
        else {
            // Нужно пройтись и удалить все кроме маски
            for(let i = measurementChartView.count - 1; i >= 0; --i) {
                var s = measurementChartView.series(i)
                if(s !== maskSeries) {      // Удалим все кроме маски
                    measurementChartView.removeSeries(s)
                }
            }
        }
    }

    function removeMaskSeries() {
        if(maskSeries == null)
            return
        measurementChartView.removeSeries(maskSeries)
        maskSeries.destroy()    //@N Чтобы удалить lowerSeries.. Не знаю как правильно..
        maskSeries = null
    }

    function updateSeriesListData() {
        if(model == null)
            return

        for(let i = 0, ind = 0; i < measurementChartView.count; ++i, ++ind) {
            let s = measurementChartView.series(i)
            if(s !== maskSeries) {
                model.initializeSeriesFromModel(s, ind)
            }
            else {
                --ind   // Тк индекс у данных будет на 1 меньше из-за маски..
            }
        }
    }

    function updateMaskSeriesData() {
        if(maskSeries == null || model == null)
            return
        model.initializeSeriesFromMask(maskSeries.upperSeries, maskSeries.lowerSeries)
    }

    function updateDomain() {
        let count = 0;
        let minX = Number.MAX_VALUE; let maxX = Number.MIN_VALUE
        let minY = Number.MAX_VALUE; let maxY = Number.MIN_VALUE

        for(let i = 0; i < measurementChartView.count; ++i) {
            let s = measurementChartView.series(i)
            if(s.visible) {
                if(s !== maskSeries) {
                    // Если данные
                    for(var j = 0; j < s.count; ++j) {
                        ++count     // Что-то точно есть..

                        var value = s.at(j)
                        minX = Math.min(minX, value.x)
                        maxX = Math.max(maxX, value.x)
                        minY = Math.min(minY, value.y)
                        maxY = Math.max(maxY, value.y)
                    }
                }
                else {
                    // Если маска
                    if(s.lowerSeries === null) {
                        // Если нет
                        for(j = 0; j < s.upperSeries.count; ++j) {
                            ++count     // Что-то точно есть..

                            value = s.upperSeries.at(j)
                            minY = Math.min(minY, value.y)
                            maxY = Math.max(maxY, value.y)

                            if(measurementChartView.count == 1) {   // Значит есть только маска.. Нужно обновить границу по Х
                                minX = Math.min(minX, value.x)
                                maxX = Math.max(maxX, value.x)
                            }
                        }
                    }
                    else {
                        // Если есть нижняя граница
                        for(j = 0; j < s.upperSeries.count; ++j) {
                            ++count     // Что-то точно есть..

                            value = s.upperSeries.at(j)
                            maxY = Math.max(maxY, value.y)
                            if(measurementChartView.count == 1) {   // Значит есть только маска.. Нужно обновить границу по Х
                                minX = Math.min(minX, value.x)
                                maxX = Math.max(maxX, value.x)
                            }
                        }

                        for(j = 0; j < s.lowerSeries.count; ++j) {
                            ++count     // Что-то точно есть..

                            value = s.lowerSeries.at(j)
                            minY = Math.min(minY, value.y)
                            if(measurementChartView.count == 1) {   // Значит есть только маска.. Нужно обновить границу по Х
                                minX = Math.min(minX, value.x)
                                maxX = Math.max(maxX, value.x)
                            }
                        }
                    }
                }
            }
        }

        if(count === 0) {
            minX = 1; maxX = 10
            minY = 1; maxY = 10
        }
        else {
            // Расширим границы
            let diff = maxY - minY
            let offset = 0
            if(diff === 0) {    //@N
                // одинаковые
                if(maxY === 0) offset = 0.1
                else offset = maxY / 10
            }
            else offset = diff / 10

            if(currentAxisY instanceof ValueAxis) {
                minY -= offset
            }
            else {
                if(minY - offset <= 0) {
                    minY -= minY / 10
                }
                else {
                    minY -= offset
                }
            }
            maxY += offset
        }

        valueAxisX.min = minX
        valueAxisX.max = maxX
        if(currentAxisY != null) {  // Не должно быть такого, но перестраховка
            currentAxisY.min = minY
            currentAxisY.max = maxY
        }
    }
}
