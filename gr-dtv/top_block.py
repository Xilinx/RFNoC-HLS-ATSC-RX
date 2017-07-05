#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Sun May 21 13:02:04 2017
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
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import atsc_rx
import ettus
import sys
from gnuradio import qtgui


class top_block(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Top Block")
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

        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.device3 = variable_uhd_device3_0 = ettus.device3(uhd.device_addr_t( ",".join(('type=x300', "")) ))
        self.samp_rate = samp_rate = 32000

        ##################################################
        # Blocks
        ##################################################
        self.uhd_rfnoc_streamer_fifo_0 = ettus.rfnoc_generic(
            self.device3,
            uhd.stream_args( # TX Stream Args
                cpu_format="f32",
                otw_format="f32",
                args="gr_vlen={0},{1}".format(2, "" if 2 == 1 else "spp={0}".format(2)),
            ),
            uhd.stream_args( # RX Stream Args
                cpu_format="f32",
                otw_format="f32",
                args="gr_vlen={0},{1}".format(2, "" if 2 == 1 else "spp={0}".format(2)),
            ),
            "FIFO", -1, -1,
        )
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_float*2, '/home/switchlanez/working_dir/gr-dtv/big_captures/rx_filt2pll_ccf', True)
        self.blocks_file_sink_1 = blocks.file_sink(gr.sizeof_float*1, '/home/switchlanez/working_dir/gr-dtv/big_captures/fpll', False)
        self.blocks_file_sink_1.set_unbuffered(False)
        self.atsc_rx_fpll_0 = atsc_rx.fpll(
                  self.device3,
                  uhd.stream_args( # TX Stream Args
                        cpu_format="f32",
                        otw_format="f32",
                        args="gr_vlen=2",
                  ),
                  uhd.stream_args( # RX Stream Args
                        cpu_format="f32",
                        otw_format="f32",
                        args="gr_vlen=1",
                  ),
                  -1,
                  -1
          )

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_file_source_0, 0), (self.uhd_rfnoc_streamer_fifo_0, 0))
        self.connect((self.atsc_rx_fpll_0, 0), (self.blocks_file_sink_1, 0))
        self.device3.connect(self.uhd_rfnoc_streamer_fifo_0.get_block_id(), 0, self.atsc_rx_fpll_0.get_block_id(), 0)

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_variable_uhd_device3_0(self):
        return self.variable_uhd_device3_0

    def set_variable_uhd_device3_0(self, variable_uhd_device3_0):
        self.variable_uhd_device3_0 = variable_uhd_device3_0

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate


def main(top_block_cls=top_block, options=None):

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
