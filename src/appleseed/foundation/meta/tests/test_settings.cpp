
//
// This source file is part of appleseed.
// Visit http://appleseedhq.net/ for additional information and resources.
//
// This software is released under the MIT license.
//
// Copyright (c) 2010 Francois Beaune
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

// appleseed.foundation headers.
#include "foundation/utility/containers/dictionary.h"
#include "foundation/utility/log.h"
#include "foundation/utility/settings.h"
#include "foundation/utility/test.h"

// Standard headers.
#include <fstream>
#include <memory>
#include <sstream>
#include <string>

FOUNDATION_TEST_SUITE(Foundation_Utility_SettingsFileReader)
{
    using namespace foundation;
    using namespace std;

    struct Fixture
    {
        Logger                  m_logger;
        SettingsFileReader      m_reader;
        Dictionary              m_dictionary;

        Fixture()
          : m_reader(m_logger)
        {
        }

        bool read(const char* filename)
        {
            return m_reader.read(filename, "../schemas/settings.xsd", m_dictionary);
        }
    };

    FOUNDATION_TEST_CASE_WITH_FIXTURE(Read_GivenEmptySettingsFile_ReturnsEmptyDictionary, Fixture)
    {
        const bool succeeded = read("data/test_settings_emptysettingsfile.xml");
        FOUNDATION_ASSERT_TRUE(succeeded);
        
        FOUNDATION_EXPECT_TRUE(m_dictionary.empty());
    }

    FOUNDATION_TEST_CASE_WITH_FIXTURE(Read_GivenSettingsFileWithTwoScalarParameters_ReturnsDictionaryWithTwoScalarParameters, Fixture)
    {
        const bool succeeded = read("data/test_settings_settingsfilewithtwoscalarparameters.xml");
        FOUNDATION_ASSERT_TRUE(succeeded);
        
        FOUNDATION_ASSERT_EQ(2, m_dictionary.strings().size());

        FOUNDATION_EXPECT_EQ(42, m_dictionary.get<int>("x"));
        FOUNDATION_EXPECT_EQ("foo", m_dictionary.get<string>("y"));
    }

    FOUNDATION_TEST_CASE_WITH_FIXTURE(Read_GivenSettingsFileWithTwoDictionaryParameters_ReturnsDictionaryWithTwoDictionaryParameters, Fixture)
    {
        const bool succeeded = read("data/test_settings_settingsfilewithtwodictionaryparameters.xml");
        FOUNDATION_ASSERT_TRUE(succeeded);

        FOUNDATION_ASSERT_EQ(0, m_dictionary.strings().size());
        FOUNDATION_ASSERT_EQ(2, m_dictionary.dictionaries().size());

        const Dictionary& sub1 = m_dictionary.dictionaries().get("sub1");
        FOUNDATION_EXPECT_EQ(42, sub1.get<int>("x"));
        FOUNDATION_EXPECT_EQ("foo", sub1.get<string>("y"));

        const Dictionary& sub2 = m_dictionary.dictionaries().get("sub2");
        FOUNDATION_EXPECT_EQ("aa", sub2.get<string>("a"));
        FOUNDATION_EXPECT_EQ("bb", sub2.get<string>("b"));
    }
}

FOUNDATION_TEST_SUITE(Foundation_Utility_SettingsFileWriter)
{
    using namespace foundation;
    using namespace std;

    bool load_file(const string& filename, string& contents)
    {
        ifstream file(filename.c_str());

        if (!file.is_open())
            return false;

        stringstream sstr;
        sstr << file.rdbuf();
        contents = sstr.str();

        return true;
    }

    bool compare_files(const string& filename1, const string& filename2)
    {
        string contents1;

        if (!load_file(filename1, contents1))
            return false;

        string contents2;
        
        if (!load_file(filename2, contents2))
            return false;

        return contents1 == contents2;
    }

    FOUNDATION_TEST_CASE(Write_GivenEmptyDictionary_WriteEmptySettingsFile)
    {
        Dictionary dictionary;

        SettingsFileWriter writer;
        writer.write("output/test_settings_emptysettingsfile.xml", dictionary);

        const bool identical =
            compare_files(
                "data/test_settings_emptysettingsfile.xml",
                "output/test_settings_emptysettingsfile.xml");

        FOUNDATION_EXPECT_TRUE(identical);
    }

    FOUNDATION_TEST_CASE(Write_GivenDictionaryWithTwoScalarParameters_WritesSettingsFileWithTwoScalarParameters)
    {
        Dictionary dictionary;
        dictionary.insert("x", 42);
        dictionary.insert("y", "foo");

        SettingsFileWriter writer;
        writer.write("output/test_settings_settingsfilewithtwoscalarparameters.xml", dictionary);

        const bool identical =
            compare_files(
                "data/test_settings_settingsfilewithtwoscalarparameters.xml",
                "output/test_settings_settingsfilewithtwoscalarparameters.xml");

        FOUNDATION_EXPECT_TRUE(identical);
    }

    FOUNDATION_TEST_CASE(Write_GivenDictionaryWithTwoDictionaryParameters_WritesSettingsFileWithTwoDictionaryParameters)
    {
        Dictionary sub1;
        sub1.insert("x", 42);
        sub1.insert("y", "foo");

        Dictionary sub2;
        sub2.insert("a", "aa");
        sub2.insert("b", "bb");

        Dictionary dictionary;
        dictionary.insert("sub1", sub1);
        dictionary.insert("sub2", sub2);

        SettingsFileWriter writer;
        writer.write("output/test_settings_settingsfilewithtwodictionaryparameters.xml", dictionary);

        const bool identical =
            compare_files(
                "data/test_settings_settingsfilewithtwodictionaryparameters.xml",
                "output/test_settings_settingsfilewithtwodictionaryparameters.xml");

        FOUNDATION_EXPECT_TRUE(identical);
    }
}
