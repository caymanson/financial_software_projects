#!/usr/bin/python
import socket
import matplotlib
import sys
import math

import matplotlib.pylab as plt
import numpy as np
from PyQt4 import QtCore, QtGui
from Ui_client import Ui_MainWindow
from time import time

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Client(QtGui.QMainWindow):
    def __init__(self, parent=None):
        QtGui.QWidget.__init__(self, parent)
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.shift = 0
        self.HOST = socket.gethostname()
        self.PORT = 0x1234
        self.MSGSIZE = 8192
        self.ifStart = False
        #self.fig = plt.figure()
        #self.axe = self.fig.add_subplot(111)
        self.start = 0
        self.elapsed = 0
        QtCore.QObject.connect(self.ui.originalBtn, QtCore.SIGNAL(_fromUtf8("clicked()")), self.calcOriginal)
        QtCore.QObject.connect(self.ui.shiftUpBtn, QtCore.SIGNAL(_fromUtf8("clicked()")), self.calcShiftUp)
        QtCore.QObject.connect(self.ui.shiftDownBtn, QtCore.SIGNAL(_fromUtf8("clicked()")), self.calcShiftDown)

        #plt.ion()
    def calc(self, bumpCurve=(0, 0, 0, 0)):
        self.start = time()
        sendMsg = "%.3f, %.3f, %.3f, %.3f, %.3f" % (self.shift, bumpCurve[0],
                                    bumpCurve[1], bumpCurve[2], bumpCurve[3])
        print sendMsg
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((self.HOST, self.PORT))
        s.send(sendMsg)
        recData = repr(s.recv(self.MSGSIZE))
        s.close()
        #recData = '''Server_Time,0.010,0.000,0.000,0.000;closing_position,-1.930,-1004035.121,17.292,1190.576,96.425,12323081.914,444.011,22644520.759;2yr_hedge,-97.723,875.449,4881.681,22478.724;shifted_position,-1.930,-1004035.121,17.292,1190.576,96.425,12323081.914,444.011,22644520.759;shifted_2yr_hedge,-97.723,875.449,4881.681,22478.724;Yield_Curve,1.000,2.000,3.000,4.000;FG,-1000.000,-0.278,0.600,100000.000,46.533,1.200,101000.000,46.811,0.600;GM,-111000.000,-30.711,0.000,-111000.000,-30.711,0.000,0.000,0.000,0.000;TT,501000.000,444.112,0.200,501000.000,444.112,0.200,0.000,0.000,0.000;XY,111000.000,95.208,0.000,111000.000,95.208,0.000,0.000,0.000,0.000;YU,1000.000,0.655,0.200,1000.000,0.655,0.200,0.000,0.000,0.000;AAA,500000.000,444.011,0.000,500000.000,444.011,0.000,0.000,0.000,0.000;AA,111000.000,95.208,0.000,111000.000,95.208,0.000,0.000,0.000,0.000;A,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000;BBB,-111000.000,-30.711,0.000,-111000.000,-30.711,0.000,0.000,0.000,0.000;BB,1000.000,0.479,1.000,1000.000,0.380,0.600,0.000,-0.098,-0.400;B,0.000,0.000,0.000,101000.000,46.910,1.000,101000.000,46.910,1.000;CCC,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000;CC,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000;C,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000;D,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000,0.000;Investment_Grade,500000.000,508.508,0.000,500000.000,508.508,0.000,0.000,0.000,0.000;Junk,1000.000,0.479,1.000,102000.000,47.290,1.600,101000.000,46.811,0.600;VaR - credit spread;'''
        #print 'Received response from the server of:', repr(recData)
        self.updateCharts(recData, bumpCurve)

    def calcOriginal(self):
        self.shift = 0
        bumpCurve = []
        for i in range(4):
            item = self.ui.maturityBucketTable.item(8, i * 2 + 2)
            bumpCurve.append(item.text().toFloat()[0])
        self.calc(bumpCurve)

    def calcShiftUp(self):
        self.shift += 0.5
        bumpCurve = []
        for i in range(4):
            item = self.ui.maturityBucketTable.item(8, i * 2 + 2)
            bumpCurve.append(item.text().toFloat()[0])
        self.calc(bumpCurve)

    def calcShiftDown(self):
        self.shift -= 0.5
        bumpCurve = []
        for i in range(4):
            item = self.ui.maturityBucketTable.item(8, i * 2 + 2)
            bumpCurve.append(item.text().toFloat()[0])
        self.calc(bumpCurve)

    def updateCharts(self, data, bumpCurve):
        parsedData = data.split(';')
        # clear previous contents
        self.ui.maturityBucketTable.clearContents()
        self.ui.dailyChangeTable.clearContents()
        self.ui.setFakeHeader(bumpCurve)
        # update Maturity Bucket Table
        tempData = parsedData[1].split(',')
        for j in range(1, len(tempData)):
                newItem = QtGui.QTableWidgetItem(tempData[j])
                self.ui.maturityBucketTable.setItem(2, j, newItem)
        tempData = parsedData[2].split(',')
        for j in range(1, len(tempData)):
                newItem = QtGui.QTableWidgetItem(tempData[j])
                self.ui.maturityBucketTable.setItem(3, j * 2, newItem)
        if not self.shift == 0:
            tempData = parsedData[3].split(',')
            for j in range(1, len(tempData)):
                    newItem = QtGui.QTableWidgetItem(tempData[j])
                    self.ui.maturityBucketTable.setItem(4, j, newItem)
            tempData = parsedData[4].split(',')
            for j in range(1, len(tempData)):
                    newItem = QtGui.QTableWidgetItem(tempData[j])
                    self.ui.maturityBucketTable.setItem(5, j * 2, newItem)
        '''
        for i in range(1, 4, 2):
            tempData = parsedData[i].split(',')
            for j in range(1, len(tempData)):
                newItem = QtGui.QTableWidgetItem(tempData[j])
                self.ui.maturityBucketTable.setItem(i + 1, j, newItem)
        for i in range(2, 5, 2):
            tempData = parsedData[i].split(',')
            for j in range(1, len(tempData)):
                newItem = QtGui.QTableWidgetItem(tempData[j])
                self.ui.maturityBucketTable.setItem(i + 1, j * 2, newItem)
        '''
        tempData = parsedData[5].split(',')
        for j in range(1, len(tempData)):
            newItem = QtGui.QTableWidgetItem(tempData[j])
            self.ui.maturityBucketTable.setItem(7, j * 2, newItem)
        self.ui.maturityBucketTable.resizeColumnsToContents()
        # update Daily Change Table
        varIndex = 0
        rowFactor = 0
        for i in range(6, len(parsedData)):
            tempData = parsedData[i].split(',')
            if tempData[0] == 'VaR-CreditSpread':
                varIndex = i
                row = self.ui.dailyChangeTable.rowCount()
                self.ui.dailyChangeTable.setRowCount(row + 1)
                rowFactor = 3
                break
            elif tempData[0] == 'AAA':
                row = self.ui.dailyChangeTable.rowCount()
                self.ui.dailyChangeTable.setRowCount(row + 1)
                rowFactor = 1
            elif tempData[0] == 'Investment_Grade':
                row = self.ui.dailyChangeTable.rowCount()
                self.ui.dailyChangeTable.setRowCount(row + 1)
                rowFactor = 2
            #if i >= 24:
            #    print 'Error, infinite loop'
            #    break
            row = self.ui.dailyChangeTable.rowCount()
            self.ui.dailyChangeTable.setRowCount(row + 1)
            for j in range(0, len(tempData)):
                newItem = QtGui.QTableWidgetItem(tempData[j])
                self.ui.dailyChangeTable.setItem(i - 4 + rowFactor, j, newItem)
        
        # update VaR
        for k in range(3):
            tempData = parsedData[varIndex].split(',')
            row = self.ui.dailyChangeTable.rowCount()
            self.ui.dailyChangeTable.setRowCount(row + 1)
            for j in range(len(tempData)):
                newItem = QtGui.QTableWidgetItem(tempData[j])
                self.ui.dailyChangeTable.setItem(varIndex - 4 + rowFactor, j, newItem)
            varIndex += 1
        self.ui.dailyChangeTable.resizeColumnsToContents()

        # update PnL bar chart
        data = [1.11, 2.11, 3.11, 4.11, 5.11, 6.11, 7.11, 8.11, 9.11, 10.11, 11.11,]
        self.elapsed = time() - self.start
        self.drawBarChart(parsedData[varIndex].split(','))
        print len(parsedData)
        print varIndex
        #self.drawBarChart(data)
        # update time
        tempData = parsedData[0].split(',')
        clientTime = self.elapsed - float(tempData[1])
        string = 'Client time: %.3f\n' % (clientTime)
        string += 'Server time: Real\tUser\tSystem\tUser+Sys\n\t%s\t' \
                '%s\t%s\t%s' % (tempData[1], tempData[2], tempData[3],
                                         tempData[4])
        print string
        textStr = QtCore.QString(string)
        self.ui.timeDisplay.setPlainText(textStr)

    def drawBarChart(self, data):
        
        N = 99
        menMeans = (20, 35, 30, 35, 27)
        menStd =   (2, 3, 4, 1, 2)

        data.pop()
        ind = np.arange(N)  # the x locations for the groups
        width = 0.35       # the width of the bars
        fig = plt.figure()
        axe = fig.add_subplot(111)
        fdata = map(float, data)
        #fdata.sort()
        scope = (fdata[len(data) - 1] - fdata[0]) / 11.0
        print 'max:%f, min:%f' % (fdata[len(data) - 1], fdata[0])
        print 'scope'
        print scope
        barData = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        for i in range(len(fdata)):
            for j in range(11):
                if math.fabs(fdata[i]) >= scope * j and math.fabs(fdata[i]) < scope * (j + 1):
                    barData[j] = barData[j] + 1
        print barData 
        #self.axe.clear()
        #data.pop(len(data))
        #rect = ax.bar(ind, data, width, color = 'b')
        rects1 = axe.bar(ind, fdata, width, color='r')

        womenMeans = (25, 32, 34, 20, 25)
        womenStd =   (3, 5, 2, 3, 3)
        #rects2 = ax.bar(ind+width, womenMeans, width, color='y', yerr=womenStd)

        #plt.draw()
        plt.show()
        # add some
        #ax.set_ylabel('Scores')
        #ax.set_title('Scores by group and gender')
        #ax.set_xticks(ind+width)
        #ax.set_xticklabels( ('G1', 'G2', 'G3', 'G4', 'G5') )

        #ax.legend( (rects1[0], rects2[0]), ('Men', 'Women') )

        #def autolabel(rects):
        #    # attach some text labels
        #    for rect in rects:
        #        height = rect.get_height()
        #        ax.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%d'%int(height),
        #                ha='center', va='bottom')

        #autolabel(rects1)
        #autolabel(rects2)
        '''
        if not self.ifStart:
            print 'show figure'
            self.ifStart = True
            plt.show()
        else:
            print 'redraw figure'
            plt.draw()
        '''
if __name__ == "__main__":
    app = QtGui.QApplication(sys.argv)
    client = Client()
    client.show()
    sys.exit(app.exec_())
