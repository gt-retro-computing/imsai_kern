import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from matplotlib.ticker import StrMethodFormatter

with open('./pc_list.txt') as f:
    data = []
    for line in f:
        if len(data) > 10 ** 4:
            break
        data.append(line)
    pcs = [int(item) for item in data]

    plt.plot(pcs)
    plt.show()
