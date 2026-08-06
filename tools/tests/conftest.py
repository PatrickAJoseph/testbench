import pytest
import yaml
from pytest_html import extras
import shutil
from pathlib import Path
from datetime import datetime

from testbench import protocol

class test_harness_data:
    def __init__(self, yaml_data):
        self.testbench_serial_port_name = yaml_data['test_bench_serial_port_name']
        self.testbench_serial_port_baud_rate = int(yaml_data['test_bench_serial_port_baud_rate'])
        self.testbench_serial_port_read_timeout = float(yaml_data['test_bench_serial_port_read_timeout'])
        self.testbench_address = int(yaml_data['test_bench_address'])

        self.user_uart_serial_port_name = yaml_data['user_uart_serial_port_name']

        self.protocol_handle = protocol.protocol(self.testbench_serial_port_name,
                                                 self.testbench_serial_port_baud_rate, 
                                                 self.testbench_address, '../test_bench.yaml', 
                                                 self.testbench_serial_port_read_timeout)

        #ADC test reports.
        self.test_adc_single_channel_frequency_test_report_file_handle = open('test_adc_single_channel_frequency_test_report.csv', 'w')
        self.test_adc_dual_channel_frequency_test_report_file_handle = open('test_adc_dual_channel_frequency_test_report.csv', 'w')

@pytest.fixture
def test_harness():

    yaml_data = []

    with open('test_config.yaml', 'r') as test_config_file:
        yaml_data = yaml.safe_load(test_config_file)

    return test_harness_data(yaml_data)

@pytest.hookimpl(optionalhook=True)
def pytest_html_results_table_header(cells):
    cells.insert(2, "<th>Description</th>")

@pytest.hookimpl(optionalhook=True)
def pytest_html_results_table_row(report, cells):
    cells.insert(2, f"<td>{getattr(report, 'description', '')}</td>")

@pytest.hookimpl(hookwrapper=True)
def pytest_runtest_makereport(item, call):
    outcome = yield
    report = outcome.get_result()

    marker = item.get_closest_marker("description")
    report.description = marker.args[0] if marker else ""