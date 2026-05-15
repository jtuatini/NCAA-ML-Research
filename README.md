# Effects of Sports Betting Markets on the College Football Playoff and End of Season AP Poll Rankings

![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?logo=cplusplus&logoColor=white)
![SPSS](https://img.shields.io/badge/SPSS-29-052FAD?logo=ibm&logoColor=white)
![License](https://img.shields.io/badge/license-MIT-green)
![University](https://img.shields.io/badge/university%20-Michigan-00274C)


Authored by Jared Tuatini, advised by Michael Mignano, Ph.D. (School of Kinesiology). Symposium poster presented April 2025; this repository extends the original 2024 analysis with an out-of-sample validation on the 2025-26 season.

## Introduction

The College Football Playoff (CFP) has experienced a substantial rise in viewership over the last decade, and following the 2024 expansion from four to twelve teams the selection process remains subject to significant public controversy. Selection is determined by a committee of voters, leaving no published quantitative threshold by which teams are deemed deserving of a playoff position. Sports betting lines, by contrast, are continuously priced markets that incorporate both public perception and on-field performance. This study evaluates whether per-game betting-line features can quantitatively reproduce the end-of-season Associated Press (AP) top-25 ranking, and by extension provide a transparent re-ranking of the teams that comprise the CFP field.

## Methods

For each of the top-25 teams in the end-of-season AP poll (Dec. 8, 2024 and Dec. 7, 2025), per-game data were collected from FantasyData covering final score, betting spread with associated odds, and total over-under (O/U). The spread for each game was weighted by its odds to an equivalent reference probability of −105:

    Weighted_Spread = Unweighted_Spread × (Odds / −105)

Per-game derived metrics (Weighted Team O/U, Weighted Opponent O/U, and point differential versus each) were then averaged over the team's regular season and conference championship games (pre-playoff only). Statistical significance of each candidate predictor was evaluated through correlation analysis and ordinary least-squares regression in SPSS. The final model was selected from the predictors showing significant individual correlation with AP rank and was used to produce a quantitative re-ranking of the AP top 25.

## Results

Weighted spread exhibited the strongest individual correlation with AP rank (p < 0.001), slightly stronger than its unweighted counterpart (p = 0.001). Points against (p = 0.003), Weighted Team O/U (p = 0.036), and Weighted Opponent O/U (p < 0.001) were also each statistically significant. In combination, the latter three features produced the most accurate multivariate model:

    AP = 0.150·PA − 0.135·Weighted_Team_O/U + 1.122·Weighted_Opp_O/U − 7.828

| Season | n | R² | Pearson r | Playoff bracket accuracy | Higher-seed baseline |
|---|---|---|---|---|---|
| 2024 (SPSS fit, in-sample) | 25 | 0.483 | 0.695 | 11 / 11 | 4 / 11 |
| 2025 (held-out test)       | 25 | 0.552 | 0.743 |  7 / 11 | 5 / 11 |

When plotted against actual AP rank, predicted values produced a slope of 0.9995 on the 2024 season (Figure 3 of the original poster). On the 2025 season, with the same coefficients applied prospectively, the model retained an R² of 0.55, a result comparable to and slightly exceeding the in-sample fit.

A re-ranking of the AP top 25 (Figure 4 of the original poster) shows continued discontinuity at the 12–13 cutoff: only five teams crossed the playoff threshold relative to the published AP poll. Among notable movements, the 2024 automatic conference-champion bids (Boise State, Arizona State) fell substantially in the re-ranking, while the eventual national champion Ohio State and the remaining top-four playoff performers rose toward the top.

## Discussion

Both Weighted Spread, Points Against, and Weighted Opponent O/U are positively correlated with AP rank, whereas Weighted Team O/U is negatively correlated. This matches predictive expectation: stronger teams are favored more heavily (negative spreads), give up fewer points, and have lower opponent O/U projections. The 2025 out-of-sample evaluation supports the original 2024 conclusion, that sports betting markets carry signal predictive of the final AP poll and the resulting playoff field, and demonstrates that the fit was not season-specific. A C++ re-ranking implementation (`src/classifier.cpp`) produced 11 / 11 correct CFP head-to-head predictions for 2024 and 7 / 11 for 2025, in both cases exceeding the trivial baseline of defaulting to the higher seed (4 / 11 and 5 / 11 respectively). Notably, the higher-seed baseline performed below chance in 2024 and only marginally above it in 2025, reflecting an increase in single-elimination upsets following the playoff's expansion from four to twelve teams.

The principal limitation remains the small sample (n = 25 per season). No individual coefficient clears the conventional p < 0.05 threshold in the multivariate model, though the overall F-test is significant at p = 0.003. Natural extensions include re-fitting on the combined 50-team dataset, switching the playoff-prediction variant from linear to logistic regression, and incorporating broader feature sets.

## Repository

```
ml-model/
├── data/                              # source xlsx + CSVs
├── src/
│   ├── classifier.cpp                 # C++ re-ranker — reads season summary from stdin
└── results/
    ├── figures/                       # generated plots
    └── spss_output/                   # original SPSS regression PDFs
```

## Build and Run

```
make release                          # builds ./classifier
make run-2024                         # re-rank the 2024 AP top 25
make run-2025                         # re-rank the 2025 AP top 25
```

## References

All historical sports betting data sourced from FantasyData.
*FantasyData — Fantasy Rankings, DFS Tools, Player Stats.* https://fantasydata.com/

## License

MIT — see `LICENSE`.
