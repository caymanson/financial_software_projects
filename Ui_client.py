from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_MainWindow(object):
    def setFakeHeader(self, bumpCurve=(0, 0, 0, 0)):
        mbtLabels = ('Book', 'Risk', 'Mkt Value', 'Risk', 'Mkt Value', 'Risk',
                     'Mkt Value', 'Risk', 'Mkt Value')
        self.maturityBucketTable.setColumnCount(len(mbtLabels))
        self.maturityBucketTable.setHorizontalHeaderLabels(mbtLabels)
        self.maturityBucketTable.verticalHeader().setVisible(False)
        self.maturityBucketTable.horizontalHeader().setVisible(False)
        self.maturityBucketTable.setRowCount(9)
        firstLineLabels = ('2YR', '5YR', '10YR', '30YR')
        newItem = QtGui.QTableWidgetItem('Book')
        self.maturityBucketTable.setItem(0, 0, newItem)
        for i in range(len(firstLineLabels)):
            newItem = QtGui.QTableWidgetItem(firstLineLabels[i])
            
            self.maturityBucketTable.setItem(0, 2 * i + 1, newItem)
            self.maturityBucketTable.setSpan(0, 2 * i + 1, 1, 2)
        for i in range(1, len(mbtLabels)):
            newItem = QtGui.QTableWidgetItem(mbtLabels[i])
            self.maturityBucketTable.setItem(1, i, newItem)
        itemLabels = ('Closing Postion', '2yr Hedge', 'Shifted Postion',
                      'Shifted 2yr Hedge')
        for i in range (len(itemLabels)):
            newItem = QtGui.QTableWidgetItem(itemLabels[i])
            self.maturityBucketTable.setItem(i + 2, 0, newItem)
        newItem = QtGui.QTableWidgetItem('Yield Curve')
        self.maturityBucketTable.setItem(7, 0, newItem)
        newItem = QtGui.QTableWidgetItem('Bump Curve')
        self.maturityBucketTable.setItem(8, 0, newItem)
        for i in range(len(bumpCurve)):
            string = "%.3f" % bumpCurve[i]
            newItem = QtGui.QTableWidgetItem(string)
            self.maturityBucketTable.setItem(8, i * 2 + 2, newItem)
        self.maturityBucketTable.resizeColumnsToContents()

        dctLabels = ('Ticker', 'Notional', 'Risk', 'LGD', 'Notional', 'Risk',
                     'LGD', 'Notional', 'Risk', 'LGD')
        self.dailyChangeTable.setColumnCount(len(dctLabels))
        self.dailyChangeTable.setHorizontalHeaderLabels(dctLabels)
        self.dailyChangeTable.verticalHeader().setVisible(False)
        self.dailyChangeTable.horizontalHeader().setVisible(False)
        self.dailyChangeTable.setRowCount(2)
        newItem = QtGui.QTableWidgetItem('Opening Position')
        #newItem.setTextAlignment(0x0004 | 0x0080)
        self.dailyChangeTable.setItem(0, 1, newItem)
        self.dailyChangeTable.setSpan(0, 1, 1, 3)
        newItem = QtGui.QTableWidgetItem('Closing Position')
        #newItem.setTextAlignment(0x0004 | 0x0080)
        self.dailyChangeTable.setItem(0, 4, newItem)
        self.dailyChangeTable.setSpan(0, 4, 1, 3)
        newItem = QtGui.QTableWidgetItem('Intra-day Change')
        #newItem.setTextAlignment(0x0004 | 0x0080)
        self.dailyChangeTable.setItem(0, 7, newItem)
        self.dailyChangeTable.setSpan(0, 7, 1, 3)
        for i in range(len(dctLabels)):
            newItem = QtGui.QTableWidgetItem(dctLabels[i])
            self.dailyChangeTable.setItem(1, i, newItem)
        self.dailyChangeTable.resizeColumnsToContents()

    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(1440, 850)
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "MainWindow", None, QtGui.QApplication.UnicodeUTF8))
        MainWindow.setAutoFillBackground(True)

        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))

        self.maturityBucketTableTitle = QtGui.QLabel(self.centralwidget)
        self.maturityBucketTableTitle.setGeometry(QtCore.QRect(880, 5, 400, 40))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        font.setPointSize(14)
        font.setBold(True)
        font.setWeight(75)
        self.maturityBucketTableTitle.setFont(font)
        self.maturityBucketTableTitle.setText(QtGui.QApplication.translate("MainWindow", "Risk by Maturity Bucket (amounts in 000\'s)", None, QtGui.QApplication.UnicodeUTF8))
        self.maturityBucketTableTitle.setObjectName(_fromUtf8("maturityBucketTableTitle"))

        self.maturityBucketTable = QtGui.QTableWidget(self.centralwidget)
        self.maturityBucketTable.setGeometry(QtCore.QRect(750, 55, 660, 320))
        self.maturityBucketTable.setObjectName(_fromUtf8("maturityBucketTable"))
        self.maturityBucketTable.setAutoFillBackground(True)
        self.maturityBucketTable.setAlternatingRowColors(True)

        self.dailyChangeTableTitle = QtGui.QLabel(self.centralwidget)
        self.dailyChangeTableTitle.setGeometry(QtCore.QRect(180, 5, 390, 40))
        font = QtGui.QFont()
        font.setFamily(_fromUtf8("Arial"))
        font.setPointSize(14)
        font.setBold(True)
        font.setWeight(75)
        self.dailyChangeTableTitle.setFont(font)
        self.dailyChangeTableTitle.setText(QtGui.QApplication.translate("MainWindow", "Daily Change by Issuer (amounts in 000\'s)", None, QtGui.QApplication.UnicodeUTF8))
        self.dailyChangeTableTitle.setObjectName(_fromUtf8("dailyChangeTableTitle"))

        self.dailyChangeTable = QtGui.QTableWidget(self.centralwidget)
        self.dailyChangeTable.setGeometry(QtCore.QRect(10, 55, 720, 750))
        self.dailyChangeTable.setLocale(QtCore.QLocale(QtCore.QLocale.English, QtCore.QLocale.UnitedStates))
        self.dailyChangeTable.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.dailyChangeTable.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAsNeeded)
        self.dailyChangeTable.setObjectName(_fromUtf8("dailyChangeTable"))
        self.dailyChangeTable.setAutoFillBackground(True)
        self.dailyChangeTable.setAlternatingRowColors(True)

        self.setFakeHeader()

        self.shiftLabel = QtGui.QLabel(self.centralwidget)
        self.shiftLabel.setGeometry(QtCore.QRect(780, 420, 141, 20))
        self.shiftLabel.setText(QtGui.QApplication.translate("MainWindow", "Whole Book Shift:", None, QtGui.QApplication.UnicodeUTF8))
        self.shiftLabel.setObjectName(_fromUtf8("shiftLabel"))

        self.shiftUpBtn = QtGui.QPushButton(self.centralwidget)
        self.shiftUpBtn.setGeometry(QtCore.QRect(900, 420, 121, 23))
        self.shiftUpBtn.setText(QtGui.QApplication.translate("MainWindow", "Shift 50% Up", None, QtGui.QApplication.UnicodeUTF8))
        self.shiftUpBtn.setObjectName(_fromUtf8("shiftUpBtn"))

        self.shiftDownBtn = QtGui.QPushButton(self.centralwidget)
        self.shiftDownBtn.setGeometry(QtCore.QRect(1030, 420, 121, 23))
        self.shiftDownBtn.setText(QtGui.QApplication.translate("MainWindow", "Shift 50% Down", None, QtGui.QApplication.UnicodeUTF8))
        self.shiftDownBtn.setObjectName(_fromUtf8("shiftDownBtn"))

        self.originalBtn = QtGui.QPushButton(self.centralwidget)
        self.originalBtn.setGeometry(QtCore.QRect(1180, 420, 121, 23))
        self.originalBtn.setText(QtGui.QApplication.translate("MainWindow", "Original", None, QtGui.QApplication.UnicodeUTF8))
        self.originalBtn.setObjectName(_fromUtf8("originalBtn"))

        self.timeDisplay = QtGui.QTextEdit(self.centralwidget)
        self.timeDisplay.setGeometry(QtCore.QRect(750, 500, 481, 91))
        self.timeDisplay.setReadOnly(True)
        self.timeDisplay.setObjectName(_fromUtf8("timeDisplay"))
        self.timeDisplay.setTextColor(QtGui.QColor(255, 0, 0))

        MainWindow.setCentralWidget(self.centralwidget)
        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QtGui.QMenuBar(MainWindow)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 1440, 23))
        self.menubar.setObjectName(_fromUtf8("menubar"))
        MainWindow.setMenuBar(self.menubar)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        pass

if __name__ == "__main__":
    import sys
    app = QtGui.QApplication(sys.argv)
    MainWindow = QtGui.QMainWindow()
    ui = Ui_MainWindow()
    ui.setupUi(MainWindow)
    MainWindow.show()
    sys.exit(app.exec_())
