import subprocess

import json
import unittest


def parse_json():
    with open("./test_cases.json") as json_data:
        d = json.load(json_data)
        return d


def make_test_case(data):
    class TestEdgeDef(unittest.TestCase):
        def test_detect_mandrel_edge(self):
            direct_output = subprocess.check_output("python3" + " " + data["Name"] + " " + data["Input"], shell=True)
            self.assertMultiLineEqual(direct_output.decode("utf-8"), str(data["Output"]))

    return TestEdgeDef


class Case1(make_test_case(dict(parse_json())["Case1"])):
    pass


class Case2(make_test_case(dict(parse_json())["Case2"])):
    pass


if __name__ == '__main__':
    unittest.main()
