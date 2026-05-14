#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <getopt.h>

using namespace std;

struct Options
{
    bool placeholder = true;
}; // Options{}

class Stats
{
private:
    unordered_map<string, size_t> headers;
    vector<double> data;

public:
    Stats() {};

    Stats(const unordered_map<string, size_t> &headers)
        : headers(headers)
    {
        read_data();
    }

    void read_data()
    {
        string line;
        getline(cin, line);
        stringstream ss(line);
        string cell = "";
        while (getline(ss, cell, ','))
        {
            stringstream cs(cell);
            double temp = 0.0;
            if (cs >> temp)
            {
                data.push_back(temp);
            }
            else
            {
                data.push_back(-1);
            }
        }
    }

    double get(const string &col) const
    {
        return data[headers.at(col)];
    }
};

class Model
{
private:
    unordered_map<string, size_t> column_headers;
    unordered_map<string, Stats> data;
    unordered_map<string, double> regression_vals;

public:
    Model()
    {
        read_season_summary();
    };

    void read_headers()
    {
        string line;
        getline(cin, line);
        stringstream ss(line);
        string header;
        getline(ss, header, ',');
        size_t idx = 0;
        while (getline(ss, header, ','))
        {
            column_headers[header] = idx++;
        }
    }

    void read_season_summary()
    {
        read_headers();
        string team_name;
        while (getline(cin, team_name, ','))
        {
            data[team_name] = Stats(column_headers);
        }
    }

    double supervised_regression_ap(const double &avg_pa, const double &avg_weighted_team_ou,
                                    const double &avg_weighted_opp_ou) const
    {
        return 0.150 * avg_pa - 0.135 * avg_weighted_team_ou + 1.122 * avg_weighted_opp_ou - 7.828;
    }

    double regression_from_stats(const Stats &stats) const
    {
        return supervised_regression_ap(stats.get("avg_pa"), stats.get("avg_weighted_team_ou"),
                                        stats.get("avg_weighted_opp_ou"));
    }

    void calc_regression_vals()
    {
        for (auto &team : data)
        {
            regression_vals[team.first] = regression_from_stats(team.second);
        }
    }

    void print_regressions() const
    {
        vector<pair<string, double>> sorted;
        size_t rank = 1;
        for (auto &team : regression_vals)
        {
            sorted.push_back({team.first, team.second});
        }
        sort(sorted.begin(), sorted.end(), [](const auto &a, const auto &b)
             { return a.second < b.second; });
        for (auto &team : sorted)
        {
            cout << rank++ << ". ";
            cout << team.first << ": " << team.second << "\n";
        }
        cout << flush;
    }

    void run_regression_analysis()
    {
        calc_regression_vals();
        print_regressions();
    }
};

void printHelp(char *command)
{
    cout << "Usage: " << command << "-h\n";
    cout << "\n"
         << flush;
} // printHelp()

void getOptions(int argc, char **argv, Options &options)
{
    opterr = static_cast<int>(false);
    int choice = 0;
    int index = 0;
    options.placeholder = true;

    option longOptions[] = {
        {"help", no_argument, nullptr, 'h'},
        {nullptr, 0, nullptr, '\0'},
    }; // longOptions[]

    while ((choice = getopt_long(argc, argv, "h", static_cast<option *>(longOptions), &index)) != -1)
    {
        switch (choice)
        {
        case 'h':
        {
            printHelp(*argv);
            exit(0);
        }

        default:
            cerr << "Unknown command line option\n"
                 << flush;
            exit(1);

        } // switch ..choice
    } // while
} // getOptions()

int main(int argc, char *argv[])
{
    std::ios_base::sync_with_stdio(false);
    cout.precision(4);

    Options options;

    getOptions(argc, argv, options);

    Model model;

    model.run_regression_analysis();
}
