# fun_client

Fun Client for a Matching Engine 

1. Number of instruments: 10 - 30
2. Number of requests (AddOrder, CancelOrder): 1-10m
3. An instrument file would be provided: Please refer "data/instrument_list.csv"
4. The measurement with any replies taking 30 seconds would be disqualified, or wrong results.
5. TradeId, OrderId cannot take the zero values.
6. AddOrderAccepted must be sent after all the trades are sent
7. 5 measurements. The one with best score would be used as final score.
8. Scoring: Overall timer value of: mean + 2.576 * sd (i.e. upper value of 99% confidence level)
9. p-cap latency would be used if possible (otherwise results from fun-score)
10. Matching engine will be run in tyrion. It has 8? cores and 64GB ram?. And fun-client will use one of them? And other
    cores can be pinned by matching engine
11. Matching engine will be started, and once the fun-client can connect to it, the measure starts.