#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Receive ATSC from UHD
# Generated: Sat Feb  4 14:31:41 2017
##################################################

from gnuradio import blocks
from gnuradio import dtv
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser


class file_atsc_rx(gr.top_block):

    def __init__(self):
        gr.top_block.__init__(self, "Receive ATSC from UHD")

        ##################################################
        # Variables
        ##################################################
        self.sps = sps = 1.1
        self.atsc_sym_rate = atsc_sym_rate = 4.5e6/286*684
        self.sample_rate = sample_rate = 6.25e6
        self.oversampled_rate = oversampled_rate = atsc_sym_rate*sps
        self.gain = gain = 18
        self.freq = freq = 605e6
        self.duration = duration = 30
        self.antenna = antenna = "TX/RX"

        ##################################################
        # Blocks
        ##################################################
        self.dtv_atsc_rx_0 = dtv.atsc_rx(sample_rate,1.5)
        self.blocks_interleaved_short_to_complex_0 = blocks.interleaved_short_to_complex(False, False)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_short*1, '/media/switchlanez/Shared Volume/DTV/captures/195 MHz/atsc_iq.sc16', False)
        self.blocks_file_sink_0 = blocks.file_sink(gr.sizeof_char*1, 'mpeg.ts', False)
        self.blocks_file_sink_0.set_unbuffered(False)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.blocks_file_source_0, 0), (self.blocks_interleaved_short_to_complex_0, 0))
        self.connect((self.blocks_interleaved_short_to_complex_0, 0), (self.dtv_atsc_rx_0, 0))
        self.connect((self.dtv_atsc_rx_0, 0), (self.blocks_file_sink_0, 0))

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

    def get_oversampled_rate(self):
        return self.oversampled_rate

    def set_oversampled_rate(self, oversampled_rate):
        self.oversampled_rate = oversampled_rate

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain

    def get_freq(self):
        return self.freq

    def set_freq(self, freq):
        self.freq = freq

    def get_duration(self):
        return self.duration

    def set_duration(self, duration):
        self.duration = duration

    def get_antenna(self):
        return self.antenna

    def set_antenna(self, antenna):
        self.antenna = antenna


def main(top_block_cls=file_atsc_rx, options=None):

    tb = top_block_cls()
    tb.start()
    tb.wait()


if __name__ == '__main__':
    main()
