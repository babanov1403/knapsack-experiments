#include <bits/stdc++.h>

// #include "babanov_tqdm.hpp"

constexpr std::size_t kTestNum = 2;

const std::filesystem::path kDataPath 
    = std::filesystem::path("data") / std::filesystem::path("02.public");

struct Item {
    std::int64_t cost;
    std::int64_t weight;
    std::int64_t idx;
};
using Items = std::vector<Item>;

struct Result {
    std::int64_t value;
    std::int64_t weight;
    std::vector<std::int64_t> idxes;

    void concat(Result& other) {
        value += other.value;
        weight += other.weight;
        for (auto x : other.idxes) {
            idxes.emplace_back(x);
        }
        auto tmp = std::vector<std::int64_t>{};
        std::swap(tmp, other.idxes);
        other.value = 0;
        other.weight = 0;
    }
};

template <bool IsForContest = false>
std::tuple<std::int64_t, std::int64_t, std::vector<Item>> read() {
    if constexpr (IsForContest) {
        std::int64_t number, max_weight;
        std::cin >> number >> max_weight;
        std::vector<Item> items(number);
        std::int64_t idx = 1;
        for (auto& [c, w, index] : items) {
            std::cin >> c >> w;
            index = idx++;
        }
        return std::make_tuple(number, max_weight, std::move(items));
    } else {
        std::filesystem::path path = kDataPath;
        std::fstream stream(path);
        std::int64_t number, max_weight;
        stream >> number >> max_weight;
        std::vector<Item> items(number);
        std::int64_t idx = 1;
        for (auto& [c, w, index] : items) {
            stream >> c >> w;
            index = idx++;
        }
        return std::make_tuple(number, max_weight, std::move(items));
    }
}

template <bool IsForContest = false>
void output(const Result& result) {
    if constexpr (IsForContest) {
        std::cout << result.value << '\n';
        for (auto idx : result.idxes) {
            std::cout << idx << ' ';
        }
        std::cout << '\n';
    } else {
        std::cout << "\n==================RESULT==================\n";
        std::cout << ":\n";
        std::cout << "VALUE: " << result.value << '\n';
        std::cout << "TOTAL WEIGTH: " << result.weight << '\n'; 
        std::cout << "ITEMS SIZE: " << result.idxes.size() << '\n';
        // std::unordered_set<std::int64_t> st(result.idxes.begin(), result.idxes.end());
        // std::cout << st.size() << " " << result.idxes.size() << '\n';
        // for (auto idx : result.idxes) {
        //     std::cout << idx << ' ';
        // }
        constexpr std::int64_t kOptAns = 31254410;
        std::cout << '\n' << "DIFFERS FROM OPT: " << 1. - result.value * 1. / kOptAns << '\n';
        std::cout << "\n==================RESULT==================n";
    }
}

Result dp_fast(std::int64_t mW, const std::vector<Item>& items) {
    std::int64_t n = items.size();
    std::vector<std::int64_t> dp(mW + 1, 0);
    std::vector<std::int64_t> dp_prev(mW + 1, 0);
    for (int k = 0; k < n; k++) {
    // for (auto k : BabanovTqdm(n)) {
        auto [icost, iw, _] = items[k];
        for (std::int64_t w = 0; w <= mW; w++) {
            dp[w] = dp_prev[w];
            if (w - iw >= 0) {
                dp[w] = std::max(dp_prev[w - iw] + icost, dp[w]);
            }
        }
        std::swap(dp, dp_prev);
    }

    return Result{std::max(dp.back(), dp_prev.back()), -1, {}};
}

auto normalize_indexes(std::vector<Item>&& items) {
    std::vector<std::int64_t> func(items.size() + 1);
    for (std::int64_t idx = 0; idx < items.size(); idx++) {
        func[idx + 1] = items[idx].idx;
        items[idx].idx = idx + 1;
    }
    return std::make_pair(std::move(items), std::move(func));
}

Result dp(std::int64_t mW, std::vector<Item>&& input_items) {
    std::int64_t n = input_items.size();
    auto [items, true_idxes] = normalize_indexes(std::move(input_items));
    std::int64_t cols = mW + 1;
    std::vector<std::int32_t> dp((mW + 1) * (n + 1), 0); // N x M => [idx][jdx] = [idx * M + jdx]
    for (int k = 1; k <= n; k++) {
        auto [icost, iw, _] = items[k - 1];
        for (std::int64_t w = 0; w <= mW; w++) {
            dp[k * cols + w] = dp[(k - 1) * cols + w];
            if (w - iw >= 0) {
                dp[k * cols + w] = std::max(dp[(k - 1) * cols + w - iw] + static_cast<std::int32_t>(icost), dp[k * cols + w]);
            }
        }
    }

    std::int64_t max_val = dp.back();
    std::vector<std::int64_t> selected;
    std::int64_t w = mW;
    std::int64_t total_w = 0;
    for (std::int64_t idx = n; idx >= 1; idx--) {
        if (dp[idx * cols + w] != dp[(idx - 1) * cols + w]) {
            selected.push_back(true_idxes[idx]);
            total_w += items[idx - 1].weight;
            w -= items[idx - 1].weight;
        }
    }
    return Result(max_val, total_w, std::move(selected));
}

Result greedy_scaled(std::int64_t mW, std::vector<Item> items) {
    std::ranges::sort(items, std::greater<>{}, [](const Item& item) {
        return item.cost * 1. / item.weight;
    });
    std::vector<std::int64_t> idxes;
    std::int64_t total_w = 0, total_c = 0;
    for (auto [c, w, idx] : items) {
        if (total_w + w <= mW) {
            total_w += w;
            total_c += c;
            idxes.emplace_back(idx);
        }
    }
    return {total_c, total_w, std::move(idxes)};
}

Result make_result_from_picked(const std::vector<Item>& picked_items) {
    std::int64_t total_c = 0;
    std::int64_t total_w = 0;
    std::vector<std::int64_t> indexes;

    for (auto [c, w, i] : picked_items) {
        total_c += c;
        total_w += w;
        indexes.emplace_back(i);
    }

    return Result(total_c, total_w, std::move(indexes));
}

auto median_exp(const Items& items) {
    double median = 0;
    for (auto [c, w, i] : items) {
        median += c * 1. / w;
    }
    return median / items.size();
}

auto split(Items&& items, std::int64_t mW) {
    double good = median_exp(items);
    double bad = 0.5 * median_exp(items);
    Items to_greedy;
    for (auto [c, w, id] : items) {
        if (c * 1. / w > good) {
            to_greedy.emplace_back(c, w, id);
            mW -= w;
            std::cout << mW << '\n';
        }
    }
    std::size_t kMaxElems = 64 * 1024ll * 1024 / mW;
    std::cout << kMaxElems << '\n';
    Items to_dp; to_dp.reserve(kMaxElems);
    Items to_pohui;
    for (auto [c, w, id] : items) {
        if (c * 1. / w > good) {
            continue;
        } else if (c * 1. / w > bad && to_dp.size() < kMaxElems) {
            to_dp.emplace_back(c, w, id);
        } else {
            to_pohui.emplace_back(c, w, id);
        }
    }
    return std::make_tuple(std::move(to_greedy), std::move(to_dp), std::move(to_pohui));
}

auto split_smart(Items&& items, std::int64_t mW) {
    std::size_t kMaxArraySize = 64 * 1024ll * 1024 / mW; // n * mW
    std::ranges::sort(items, std::greater<>{}, [](const Item& item) {
        return item.cost * 1. / item.weight;
    });
    // we consider that our space never ends while we picking up from the middle
    Items to_greedy;
    Items to_dp;
    std::int64_t idx = 0;
    for (; idx < items.size() / 2 && (to_dp.size() + 1) <= kMaxArraySize; idx++) {
        to_dp.emplace_back(items[idx]);
    }

    while (idx < items.size()) {
        if ((to_dp.size() + 1) > kMaxArraySize || items[idx].cost * 1. / items[idx].weight <= 1.) {
            std::cout << items[idx].cost << " " << items[idx].weight << '\n';
            to_greedy.emplace_back(items[idx]);
        } else {
            to_dp.emplace_back(items[idx]);
        }
        idx++;
    }
    return std::make_tuple(std::move(to_dp), std::move(to_greedy));
}

auto split_very_smart(Items&& items, std::int64_t mW) {
    std::size_t kMaxArraySize = 64 * 1024ll * 1024; // n * mW
    std::ranges::sort(items, [](const Item& lhs, const Item& rhs) {
        double ass = lhs.cost * 1. / lhs.weight;
        double other_ass = rhs.cost * 1. / rhs.weight;

        if (ass == other_ass) {
            return lhs.cost > rhs.cost;
        }
        return ass > other_ass;
    });
    Items to_greedy;
    std::int64_t total_w = 0;
    std::int64_t total_c = 0;
    std::int64_t critical_idx = -1;
    for (std::int64_t idx = 0; idx < items.size(); idx++) {
        if (items[idx].weight + total_w > mW) {
            critical_idx = idx;
            break;
        }
        total_c += items[idx].cost;
        total_w += items[idx].weight;
        to_greedy.emplace_back(items[idx]);
    }

    // now we either take items[critical_idx] and delete some one from array
    // or do not take items[critical_idx] and trying to fill other

    // 1) do not take items[critical_idx], take items to dp until we can
    mW -= total_w;
    Items to_dp;
    for (std::int64_t idx = critical_idx; idx < items.size() && to_dp.size() * mW < kMaxArraySize; idx++) {
        to_dp.emplace_back(items[idx]);
    }

    return std::make_tuple(std::move(to_greedy), std::move(to_dp));

    // 2) delete some one


}

bool validate(const Result& res, const std::vector<Item>& origin_items) {
    std::int64_t c = 0, w = 0;
    for (auto idx : res.idxes) {
        c += origin_items[idx - 1].cost;
        w += origin_items[idx - 1].weight;
    }

    if (c != res.value || w != res.weight) {
        std::cout << "ORIGIN: cost: " << c << " w: " << w << '\n';
        std::cout << "CALCULATED (may be wrong): cost: " << res.value << " w: " << res.weight << '\n';
        return false;
    }
    return true;
}

int main() {
    //niceon 31253920
    //fuckkk 31253846
    //smarty 31253846
    //greedy 31253846
    //optimy 31254410
    constexpr bool kIsForContest = true;
    // constexpr bool kIsForContest = true;
    auto [number, max_weight, items] = read<kIsForContest>();
    auto items_copy = items;

    // auto [to_greedy, to_dp, to_pohui] = split(std::move(items), max_weight);
    // auto res = GreedyScaled(max_weight, std::move(to_greedy));
    // auto lhs = DP(max_weight - res.weight, std::move(to_dp));
    // res.concat(lhs);
    // lhs = GreedyScaled(max_weight - res.weight, std::move(to_pohui));
    // res.concat(lhs);

    // auto [to_dp, to_greedy] = split_smart(std::move(items), max_weight);
    // auto res = dp(max_weight, std::move(to_dp));
    // auto lhs = greedy_scaled(max_weight - res.weight, std::move(to_greedy));
    // res.concat(lhs);

    auto [to_greedy, to_dp] = split_very_smart(std::move(items), max_weight);
    auto res = greedy_scaled(max_weight, std::move(to_greedy));
    auto lhs = dp(max_weight - res.weight, std::move(to_dp));
    res.concat(lhs);
    // std::int64_t total_elems = 0;
    // std::int64_t occupied_weight = 0;
    // auto res = Result(0, 0, {});
    // while (total_elems < number && occupied_weight < max_weight) {
    //     std::vector<Item> curr; curr.reserve(kMaxElems);
    //     while (!items.empty() && curr.size() != kMaxElems) {
    //         curr.push_back(items.back());
    //         items.pop_back();
    //     }
    //     auto inter = DP(max_weight - occupied_weight, std::move(curr));
    //     if (inter.value == 0) {
    //         break;
    //     }
    //     res.concat(inter);
    //     occupied_weight = res.weight;
    // }

    output<kIsForContest>(res);
    // auto dp_true = GreedyScaled(max_weight, items_copy);
    // Output<kIsForContest>(dp_true);
}