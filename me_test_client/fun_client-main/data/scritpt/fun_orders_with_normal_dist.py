import csv
import random
import pandas as pd
import numpy as np
from dataclasses import dataclass

# from sortedcontainers import SortedList


price_list = {
    1000001: 43200,
    1000002: 38500,
    1000003: 68500,
    1000004: 7850,
    1000005: 9050,
    1000006: 23500,
    1000007: 15200,
    1000008: 16300,
    1000009: 8500,
    1000010: 22500,

}

volume_list = {
    1000001: 50000,
    1000002: 9000,
    1000003: 15000,
    1000004: 3000,
    1000005: 8000,
    1000006: 6000,
    1000007: 9800,
    1000008: 32000,
    1000009: 15000,
    1000010: 6300,

}


def insert_order(
        csv_writer, order_id, instrument=None, volume_=None, price_=None, side=None, counter_order_id=0, order_type="A"
):
    csv_writer.writerow([order_type, order_id, instrument, volume_, price_, side, counter_order_id])
    accepted_order_type = "Y"
    if order_type == "D":
        accepted_order_type = "C"
    csv_writer.writerow([accepted_order_type, order_id, None, None, None, None, None])


class GenerateOrders:
    df_columns = ["messageType", "order_id", "instrument", "volume", "price", "side", "counterOrderId"]

    def __init__(self, security=None, number_of_orders=100, variance_factor=12, order_id=10000):
        self.counter = 0
        self.oid_to_order = {}
        self.buy = True
        self.number_of_orders = number_of_orders
        self.variance_factor = variance_factor
        self.order_id = order_id

    def get_security_details(self, security):
        security = security
        price = price_list[security]
        volume = volume_list[security]

        vol = np.random.normal(volume, 30.0, self.number_of_orders)
        pri = np.random.normal(price, 100.0, self.number_of_orders)
        security_list = [security] * self.number_of_orders
        dataset = pd.DataFrame(
            {'security': list(security_list), 'price': list(pri.astype(int)), 'volume': list(vol.astype(int))},
            columns=['security', 'price', 'volume'])

        # print(
        #     f"Volume {volume} , price {price} , pv {price_variance} vv: {volume_variance} final price "
        #     f" {pri} final {volume} .")
        return dataset

    def run_2(self):

        security_data = []
        for i in range(1000001, 1000011):
            security_data.append(self.get_security_details(i))

        security_data = pd.concat(security_data)
        security_data = security_data.sample(frac=1).reset_index(drop=True)
        order_id = self.order_id
        print(f"Generating orders.")
        with open("../data/feed_dist_5.csv", "a") as outfile:
            csv_file_writer = csv.writer(outfile, delimiter=",", quotechar="|", quoting=csv.QUOTE_MINIMAL)
            csv_file_writer.writerow(self.df_columns)
            for index, row in security_data.iterrows():

                order_id = order_id + 1

                buy = "S"

                if random.randint(1, self.number_of_orders) % 100:
                    buy = "B"
                insert_order(csv_file_writer, order_id, row['security'], row['volume'], row['price'], buy)
                self.counter += 1

                if self.counter > 25:
                    insert_order(csv_file_writer, order_id - 7, order_type="D", counter_order_id=None)
                    insert_order(csv_file_writer, order_id - 8, order_type="D", counter_order_id=None)
                    insert_order(csv_file_writer, order_id - 9, order_type="D", counter_order_id=None)
                    insert_order(csv_file_writer, order_id - 11, order_type="D", counter_order_id=None)
                    insert_order(csv_file_writer, order_id - 12, order_type="D", counter_order_id=None)
                    insert_order(csv_file_writer, order_id - 13, order_type="D", counter_order_id=None)
                    insert_order(csv_file_writer, order_id - 15, order_type="D", counter_order_id=None)
                    insert_order(csv_file_writer, order_id - 17, order_type="D", counter_order_id=None)
                    insert_order(csv_file_writer, order_id - 18, order_type="D", counter_order_id=None)
                    insert_order(csv_file_writer, order_id - 20, order_type="D", counter_order_id=None)
                    insert_order(csv_file_writer, order_id - 21, order_type="D", counter_order_id=None)
                    insert_order(csv_file_writer, order_id - 22, order_type="D", counter_order_id=None)

                    self.counter = 0

        last_order_id = order_id
        print(f" Last order generated with order_id {last_order_id}")

        return last_order_id


def main():
    app = GenerateOrders(number_of_orders=100000, variance_factor=12)
    app.run_2()


if __name__ == "__main__":
    main()
