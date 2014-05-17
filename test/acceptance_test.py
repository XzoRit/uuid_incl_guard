#!/usr/bin/python

import unittest
import subprocess
import re
import os

class TestUuidInclGuard(unittest.TestCase):

    def setUp(self):
        self.__uuid_incl_guard = "../build/uuid_incl_guard"
        self.__reUuidInclGuard = r"INCL_[\da-f]{8}_(?:[\da-f]{4}_){3}[\da-f]{8}"
        self.__simple_cpp_header = "simple_cpp_header.hpp"
        fh = open(self.__simple_cpp_header, "w")
        fh.write("class Test1\n"
                 "{\n"
                 "public:\n"
                 "  Test1();\n"
                 "private:\n"
                 "  Test1(Test1 const&);\n"
                 "  Test1& operator=(Test1 const&);\n"
                 "};\n")
        fh.close()
        self.__simple_cpp_source = "simple_cpp_source.cpp"
        fh = open(self.__simple_cpp_source, "w")
        fh.write("Test1::Test()\n"
                 "{}\n")
        fh.close()

    def tearDown(self):
        os.remove(self.__simple_cpp_header)
        os.remove(self.__simple_cpp_source)

    def test_whenCalledWithoutArgsPrintsOneIncludeGuard(self):
        inclGuard = subprocess.check_output([self.__uuid_incl_guard])
        match = re.match(self.__reUuidInclGuard, inclGuard)
        self.assertIsNotNone(match, "inclGuard {} did not match {}".format(inclGuard, self.__reUuidInclGuard))

    def test_whenCalledWithGenerate10Prints10IncludeGuards(self):
        inclGuards = subprocess.check_output([self.__uuid_incl_guard, "--generate", "10"])
        guards = inclGuards.split("\n")
        self.assertEqual(len(guards), 11) # 10 guards and an extra new-line at the end
        for guard in guards[0:9]:
            match = re.match(self.__reUuidInclGuard, guard)
            self.assertIsNotNone(match, "Guard {} did not match {}".format(guard, self.__reUuidInclGuard))

    def test_whenCalledWithInHppFileIncludeGuardIsGeneratedIntoThatFile(self):
        output = subprocess.check_output([self.__uuid_incl_guard, "--in", self.__simple_cpp_header])
        reOutput = r'"{}": has new include guard ({})'.format(self.__simple_cpp_header, self.__reUuidInclGuard)
        match = re.match(reOutput, output)
        self.assertIsNotNone(match)
        fh = open(self.__simple_cpp_header, "r")
        fileContent = fh.read()
        reInclGuard = re.compile("""#ifndef {0}
                                    \n
                                    #define {0}
                                    \n""".format(match.group(1)), re.VERBOSE | re.MULTILINE)
        match = reInclGuard.match(fileContent)
        self.assertIsNotNone(match)
        self.assertTrue(fileContent.endswith("\n#endif\n"))
        fh.close()

    def test_whenCalledWithCompanynameCopyrightNoticeIsGeneratedIntoGivenFile(self):
        company = "Xzr"
        output = subprocess.check_output([self.__uuid_incl_guard, "--company", company, "--in", self.__simple_cpp_header])
        reOutput = r'"{}": has new copyright notice with company {}'.format(self.__simple_cpp_header, company)
        match = re.search(reOutput, output)
        self.assertIsNotNone(match)
        fh = open(self.__simple_cpp_header, "r")
        fileContent = fh.read()
        self.assertTrue(fileContent.startswith(
            "/*\n"
            " * Copyright 2014 {} and Licensors. All Rights Reserved. Company Confidential.\n"
            " */\n".format(company)))

    def test_whenCalledWithInCppFileAndCompanynameCopyrightNoticeIsGeneratedIntoGivenFile(self):
        company = "Xzr"
        output = subprocess.check_output([self.__uuid_incl_guard, "--company", company, "--in", self.__simple_cpp_source])
        reOutput = r'"{}": has new copyright notice with company {}'.format(self.__simple_cpp_source, company)
        match = re.search(reOutput, output)
        self.assertIsNotNone(match)
        fh = open(self.__simple_cpp_source, "r")
        fileContent = fh.read()
        self.assertTrue(fileContent.startswith(
            "/*\n"
            " * Copyright 2014 {} and Licensors. All Rights Reserved. Company Confidential.\n"
            " */\n".format(company)))

if __name__ == '__main__':
    unittest.main()
