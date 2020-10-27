import matplotlib.pyplot as plt
import matplotlib.ticker as ticker
from matplotlib.ticker import StrMethodFormatter

with open('./pc_list.txt') as f:
    data = []
    for line in f:
        data.append(line)
    pcs = [int(item) for item in data]
    if len(pcs) > 10000:
        pcs = pcs[-10000:]

    plt.plot(pcs)
    plt.show()
