#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <cmath>
#include "csvstream.hpp"

using namespace std;

class Model
{

public:
    Model(csvstream &trainfile)
        : trainstream(&trainfile), num_posts(0) {}

    virtual void load()
    {
        map<string, string> rowdata;
        while (*trainstream >> rowdata)
        {
            string tag = rowdata["tag"];
            trainingdata[num_posts].first = tag;
            labelcount[tag] += 1;
            istringstream source(rowdata["content"]);
            string word;
            set<string> row;
            while (source >> word)
            {
                trainingdata[num_posts].second.push_back(word);
                vocab.insert(word);
                row.insert(word);
            }
            for (string s : row)
            {
                wordcount[s] += 1;
                label_words[tag][s] += 1;
            }
            ++num_posts;
        }
    }

    double log_prior(const string &label)
    {
        return log(static_cast<double>(labelcount[label]) / num_posts);
    }

    double log_likelihood(const string &label, const string &word)
    {
        if (label_words[label][word])
        {
            return log(static_cast<double>(label_words[label][word]) / labelcount[label]);
        }
        else if (vocab.find(word) != vocab.end())
        {
            return log(static_cast<double>(wordcount[word]) / num_posts);
        }
        else
        {
            return log(1.0 / num_posts);
        }
    }

    virtual void print_info()
    {
        cout << "training data:" << endl;
        for (auto data : trainingdata)
        {
            cout << "  label = " << data.second.first << ", content =";
            for (auto word : data.second.second)
            {
                cout << " " << word;
            }
            if (data.second.second.empty())
            {
                cout << " ";
            }
            cout << endl;
        }
        cout << "trained on " << num_posts << " examples" << endl;
        cout << "vocabulary size = " << vocab.size() << endl
             << endl;
        cout << "classes:" << endl;
        for (auto label : labelcount)
        {
            cout << "  " << label.first << ", " << label.second << " examples, ";
            cout << "log-prior = " << log_prior(label.first) << endl;
        }
        cout << "classifier parameters:" << endl;
        for (auto param : label_words)
        {
            for (auto word : label_words[param.first])
            {
                cout << "  " << param.first << ":" << word.first;
                cout << ", count = " << word.second;
                cout << ", log-likelihood = ";
                cout << log_likelihood(param.first, word.first) << endl;
            }
        }
        cout << endl;
    }

protected:
    csvstream *trainstream;

    map<int, pair<string, vector<string>>> trainingdata;

    int num_posts;

    map<string, int> wordcount;

    map<string, int> labelcount;

    map<string, map<string, int>> label_words;

    set<string> vocab;
};

class TestModel : public Model
{
public:
    TestModel(csvstream &trainfile, csvstream &testfile)
        : Model(trainfile), teststream(&testfile), num_tests(0), accuracy(0) {}

    void load() override
    {
        Model::load();
        map<string, string> rowdata;
        int test_num = 0;
        while (*teststream >> rowdata)
        {
            string tag = rowdata["tag"];
            istringstream source(rowdata["content"]);
            string word;
            testdata[test_num].first = tag;
            while (source >> word)
            {
                testdata[test_num].second.push_back(word);
            }
            ++test_num;
            ++num_tests;
        }
    }

    pair<string, double> log_probability(const int test)
    {
        map<string, double> probabilities;
        const string *prediction = nullptr;
        string key;
        for (auto l : labelcount)
        {
            string label = l.first;
            probabilities[label] = log_prior(label);
            set<string> uniquewords;
            for (string word : testdata[test].second)
            {
                uniquewords.insert(word);
            }
            for (string unique : uniquewords)
            {
                probabilities[label] += log_likelihood(label, unique);
            }
            if (!prediction)
            {
                key = label;
                prediction = &key;
            }
            else if (probabilities[label] > probabilities[key])
            {
                key = label;
                prediction = &key;
            }
        }
        return {key, probabilities[key]};
    }

    void predict()
    {
        vector<pair<string, double>> initpredictions(num_tests);
        for (int i = 0; i < num_tests; ++i)
        {
            initpredictions[i] = log_probability(i);
            if (initpredictions[i].first == testdata[i].first)
            {
                ++accuracy;
            }
        }
        predictions = initpredictions;
    }

    virtual void print_info() override
    {
        cout << "trained on " << num_posts << " examples" << endl
             << endl;
        cout << "test data:" << endl;
        for (int i = 0; i < predictions.size(); ++i)
        {
            cout << "  correct = " << testdata[i].first;
            cout << ", predicted = " << predictions[i].first;
            cout << ", log-probability score = " << predictions[i].second;
            cout << endl
                 << "  content =";
            for (string word : testdata[i].second)
            {
                cout << " " << word;
            }
            if (testdata[i].second.empty())
            {
                cout << " ";
            }
            cout << endl
                 << endl;
        }
        cout << "performance: " << accuracy << " / " << num_tests;
        cout << " posts predicted correctly" << endl;
    }

private:
    csvstream *teststream;

    map<int, pair<string, vector<string>>> testdata;

    int num_tests;

    vector<pair<string, double>> predictions;

    int accuracy;
};

int main(int argc, char *argv[])
{
    cout.precision(3);

    if (argc > 3)
    {
        cout << "Usage: classifier.exe TRAIN_FILE [TEST_FILE]" << endl;
        return 1;
    }
    string train_file = argv[1];
    csvstream train_data(train_file);
    if (argc == 3)
    {
        string test_file = argv[2];
        csvstream test_data(test_file);
        TestModel model(train_data, test_data);
        model.load();
        model.predict();
        model.print_info();
    }
    else
    {
        Model model(train_data);
        model.load();
        model.print_info();
    }
}