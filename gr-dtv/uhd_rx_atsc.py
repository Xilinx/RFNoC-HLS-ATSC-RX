#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Receive ATSC from UHD
# Generated: Sun May 21 14:19:47 2017
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from PyQt4.QtCore import QObject, pyqtSlot
from gnuradio import analog
from gnuradio import blocks
from gnuradio import dtv
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from optparse import OptionParser
import sys
from gnuradio import qtgui


class uhd_rx_atsc(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Receive ATSC from UHD")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Receive ATSC from UHD")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "uhd_rx_atsc")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.sps = sps = 1.1
        self.atsc_sym_rate = atsc_sym_rate = 4.5e6/286*684
        self.sample_rate = sample_rate = 6.25e6
        self.oversampled_rate = oversampled_rate = atsc_sym_rate*sps
        self.gain = gain = 18
        self.antenna = antenna = 'TX/RX'

        ##################################################
        # Blocks
        ##################################################
        self._sample_rate_tool_bar = Qt.QToolBar(self)
        self._sample_rate_tool_bar.addWidget(Qt.QLabel('Sample Rate'+": "))
        self._sample_rate_line_edit = Qt.QLineEdit(str(self.sample_rate))
        self._sample_rate_tool_bar.addWidget(self._sample_rate_line_edit)
        self._sample_rate_line_edit.returnPressed.connect(
        	lambda: self.set_sample_rate(eng_notation.str_to_num(str(self._sample_rate_line_edit.text().toAscii()))))
        self.top_grid_layout.addWidget(self._sample_rate_tool_bar, 1,2,1,1)
        self.rx_filter = dtv.atsc_rx_filter(sample_rate,sps)
        self._gain_range = Range(0, 31, 1, 18, 200)
        self._gain_win = RangeWidget(self._gain_range, self.set_gain, 'Gain', "counter_slider", float)
        self.top_grid_layout.addWidget(self._gain_win, 1,3,1,1)
        self.fpll = dtv.atsc_fpll(oversampled_rate)
        self.dtv_atsc_viterbi_decoder_0 = dtv.atsc_viterbi_decoder()
        self.dtv_atsc_sync_0 = dtv.atsc_sync(oversampled_rate)
        self.dtv_atsc_rs_decoder_0 = dtv.atsc_rs_decoder()
        self.dtv_atsc_fs_checker_0 = dtv.atsc_fs_checker()
        self.dtv_atsc_equalizer_0 = dtv.atsc_equalizer()
        self.dtv_atsc_derandomizer_0 = dtv.atsc_derandomizer()
        self.dtv_atsc_depad_0 = dtv.atsc_depad()
        self.dtv_atsc_deinterleaver_0 = dtv.atsc_deinterleaver()
        self.displays = Qt.QTabWidget()
        self.displays_widget_0 = Qt.QWidget()
        self.displays_layout_0 = Qt.QBoxLayout(Qt.QBoxLayout.TopToBottom, self.displays_widget_0)
        self.displays_grid_layout_0 = Qt.QGridLayout()
        self.displays_layout_0.addLayout(self.displays_grid_layout_0)
        self.displays.addTab(self.displays_widget_0, 'RX Spectrum')
        self.top_grid_layout.addWidget(self.displays, 0,0,1,4)
        self.dc_blocker_xx_0 = filter.dc_blocker_ff(4096, True)
        self.blocks_file_source_0_0 = blocks.file_source(gr.sizeof_gr_complex*1, '/home/switchlanez/working_dir/gr-dtv/big_captures/atsc_rx_filter_in', False)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_char*1, '/home/switchlanez/working_dir/gr-dtv/big_captures/mpeg.ts', False)
        self.blocks_file_sink_0.set_unbuffered(False)
        self._antenna_options = ('TX/RX', 'RX2', )
        self._antenna_labels = ('TX/RX', 'RX2', )
        self._antenna_group_box = Qt.QGroupBox('Antenna')
        self._antenna_box = Qt.QVBoxLayout()
        class variable_chooser_button_group(Qt.QButtonGroup):
            def __init__(self, parent=None):
                Qt.QButtonGroup.__init__(self, parent)
            @pyqtSlot(int)
            def updateButtonChecked(self, button_id):
                self.button(button_id).setChecked(True)
        self._antenna_button_group = variable_chooser_button_group()
        self._antenna_group_box.setLayout(self._antenna_box)
        for i, label in enumerate(self._antenna_labels):
        	radio_button = Qt.QRadioButton(label)
        	self._antenna_box.addWidget(radio_button)
        	self._antenna_button_group.addButton(radio_button, i)
        self._antenna_callback = lambda i: Qt.QMetaObject.invokeMethod(self._antenna_button_group, "updateButtonChecked", Qt.Q_ARG("int", self._antenna_options.index(i)))
        self._antenna_callback(self.antenna)
        self._antenna_button_group.buttonClicked[int].connect(
        	lambda i: self.set_antenna(self._antenna_options[i]))
        self.top_grid_layout.addWidget(self._antenna_group_box, 1,0,1,1)
        self.agc = analog.agc_ff(1e-5, 4.0, 1.0)
        self.agc.set_max_gain(65536)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.agc, 0), (self.dtv_atsc_sync_0, 0))
        self.connect((self.blocks_file_source_0_0, 0), (self.rx_filter, 0))
        self.connect((self.dc_blocker_xx_0, 0), (self.agc, 0))
        self.connect((self.dtv_atsc_deinterleaver_0, 0), (self.dtv_atsc_rs_decoder_0, 0))
        self.connect((self.dtv_atsc_depad_0, 0), (self.blocks_file_sink_0, 0))
        self.connect((self.dtv_atsc_derandomizer_0, 0), (self.dtv_atsc_depad_0, 0))
        self.connect((self.dtv_atsc_equalizer_0, 0), (self.dtv_atsc_viterbi_decoder_0, 0))
        self.connect((self.dtv_atsc_fs_checker_0, 0), (self.dtv_atsc_equalizer_0, 0))
        self.connect((self.dtv_atsc_rs_decoder_0, 0), (self.dtv_atsc_derandomizer_0, 0))
        self.connect((self.dtv_atsc_sync_0, 0), (self.dtv_atsc_fs_checker_0, 0))
        self.connect((self.dtv_atsc_viterbi_decoder_0, 0), (self.dtv_atsc_deinterleaver_0, 0))
        self.connect((self.fpll, 0), (self.dc_blocker_xx_0, 0))
        self.connect((self.rx_filter, 0), (self.fpll, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "uhd_rx_atsc")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps
        self.set_oversampled_rate(self.atsc_sym_rate*self.sps)

    def get_atsc_sym_rate(self):
        return self.atsc_sym_rate

    def set_atsc_sym_rate(self, atsc_sym_rate):
        self.atsc_sym_rate = atsc_sym_rate
        self.set_oversampled_rate(self.atsc_sym_rate*self.sps)

    def get_sample_rate(self):
        return self.sample_rate

    def set_sample_rate(self, sample_rate):
        self.sample_rate = sample_rate
        Qt.QMetaObject.invokeMethod(self._sample_rate_line_edit, "setText", Qt.Q_ARG("QString", eng_notation.num_to_str(self.sample_rate)))

    def get_oversampled_rate(self):
        return self.oversampled_rate

    def set_oversampled_rate(self, oversampled_rate):
        self.oversampled_rate = oversampled_rate

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain

    def get_antenna(self):
        return self.antenna

    def set_antenna(self, antenna):
        self.antenna = antenna
        self._antenna_callback(self.antenna)


def main(top_block_cls=uhd_rx_atsc, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
