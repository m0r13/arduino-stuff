#!/usr/bin/env python3

import os
import sys
import serial
import time
from PyQt5 import QtGui, QtWidgets, QtCore

SERIAL_UPDATE_EVERY = 1 / 25.0

def read_parameters():
    parameters = []
    i = 0
    with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), "src", "sketch.ino")) as f:
        for line in f.readlines():
            line = line.strip()
            if (not line.startswith("Parameter ")) or (not "(" in line) or (not ")" in line):
                continue
            index = i
            name = line[len("Parameter "):].split("(")[0]
            args = list(map(float, line[line.find("(")+1:line.find(")")].split(",")))
            description = line.split("// ")[1]
            yield index, name, args, description
            i += 1

class SerialCommunicator(QtCore.QObject):
    def __init__(self, *args, **kwargs):
        super(QtCore.QObject, self).__init__(*args, **kwargs)

        self.stopped = False
        self.serial = serial.Serial("/dev/ttyACM0", baudrate=9600)
        #self.serial = serial.Serial()
        self.changed_values = {}

    @QtCore.pyqtSlot()
    def stop(self):
        self.stopped = True

    @QtCore.pyqtSlot()
    def change_value(self, index, value):
        self.changed_values[index] = value

    @QtCore.pyqtSlot()
    def work(self):
        last_serial_update = 0
        while not self.stopped:
            time.sleep(0.01)
            if not self.serial.is_open:
                continue

            now = time.time()
            # inform arduino about changes
            if len(self.changed_values) != 0 and now - last_serial_update > SERIAL_UPDATE_EVERY:
                #print(self.changed_values)
                for value_index, value in self.changed_values.items():
                    self.serial.write(bytearray([value_index, value]))
                self.serial.flushOutput()
                self.changed_values = {}
                last_serial_update = now
            while self.serial.inWaiting() > 0:
                print(self.serial.read().decode("ascii"), end="")

class ParameterSlider(QtWidgets.QWidget):
    def __init__(self, parameter, *args, **kwargs):
        super(QtWidgets.QWidget, self).__init__(*args, **kwargs)

        self.parameter = parameter
        index, name, args, description = parameter

        self.label_name = QtWidgets.QLabel()
        self.label_min = QtWidgets.QLabel(str(args[0]))
        self.label_max = QtWidgets.QLabel(str(args[2]))

        self.slider = QtWidgets.QSlider(QtCore.Qt.Horizontal)
        self.slider.setRange(0, 1024)
        self.slider.setTracking(True)
        self.slider.valueChanged.connect(self.update_labels)
        self.reset()
        self.update_labels()

        button = QtWidgets.QPushButton("Flash!")
        button.pressed.connect(self.set_max)
        button.released.connect(self.set_min)
        has_button = name in ("stroboOverride", "stroboEnabled")

        hlayout = QtWidgets.QHBoxLayout()
        hlayout.addWidget(self.label_min)
        hlayout.addWidget(self.slider)
        hlayout.addWidget(self.label_max)
        vlayout = QtWidgets.QVBoxLayout()
        vlayout.addWidget(self.label_name)
        vlayout.addLayout(hlayout)
        if has_button:
            vlayout.addWidget(button)
            self.label_min.hide()
            self.label_max.hide()
            self.slider.hide()
        self.setLayout(vlayout)

    def reset(self):
        index, name, args, description = self.parameter
        self.slider.setValue((args[1] - args[0]) / (args[2] - args[0]) * self.slider.maximum())

    def set_min(self):
        self.slider.setValue(self.slider.minimum())

    def set_max(self):
        self.slider.setValue(self.slider.maximum())

    def update_labels(self):
        index, name, args, description = self.parameter
        alpha = self.slider.value() / 1024.0
        value = (1-alpha) * args[0] + alpha * args[2]
        self.label_name.setText("%.3f - %s" % (value, description))

class ParameterWindow(QtWidgets.QMainWindow):
    def __init__(self, parameters, *args, **kwargs):
        super(QtWidgets.QMainWindow, self).__init__(*args, **kwargs)
        self.parameters = parameters
        self.widgets = []

        self.worker_thread = QtCore.QThread()
        self.worker = SerialCommunicator()
        self.worker.moveToThread(self.worker_thread)
        self.worker_thread.start()
        QtCore.QTimer.singleShot(0, self.worker.work)

        reset = QtWidgets.QPushButton("Reset")
        reset.clicked.connect(self.reset)

        layout = QtWidgets.QVBoxLayout()
        layout.addWidget(reset)
        for parameter in self.parameters:
            print(parameter)
            widget = ParameterSlider(parameter)
            widget.slider.valueChanged.connect(self.slider_changed)
            layout.addWidget(widget)
            self.widgets.append(widget)
        layout.addStretch()

        widget = QtWidgets.QWidget()
        widget.setLayout(layout)
        self.setCentralWidget(widget)

    def closeEvent(self, event):
        self.worker.stop()
        self.worker_thread.quit()
        self.worker_thread.wait()

    def reset(self):
        for widget in self.widgets:
            widget.reset()

    def slider_changed(self):
        widget = self.sender().parent()
        index, value = widget.parameter[0], int(widget.slider.value() / widget.slider.maximum() * 255)
        self.worker.change_value(index, value)

if __name__ == "__main__":
    parameters = list(read_parameters())

    app = QtWidgets.QApplication(sys.argv)
    w = ParameterWindow(parameters)
    w.show()
    app.exec()

