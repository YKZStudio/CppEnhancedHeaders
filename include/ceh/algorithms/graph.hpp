// ceh/algorithms/graph.hpp
//
// Graph —— 加权图与常用图算法（header-only），位于 ceh::。
//
// 用邻接表表示 n 个顶点（0..n-1）的图，可有向或无向。内置 BFS / DFS 遍历、
// 单源最短路（BFS 按边数、Dijkstra 按非负权）、拓扑排序、无向图连通分量。
//
// 异常：顶点下标越界抛 std::out_of_range；对含负权边的图调用 dijkstra
//        抛 std::invalid_argument。
//
// 用法：
//     ceh::Graph g(4);              // 4 个顶点，无向
//     g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 3);
//     g.bfs_distances(0);           // {0,1,2,3}
//     ceh::Graph d(3, true);        // 有向
//     d.add_edge(0,1); d.add_edge(1,2);
//     d.topological_sort();         // {0,1,2}

#ifndef CEH_ALGORITHMS_GRAPH_HPP
#define CEH_ALGORITHMS_GRAPH_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ceh {

class Graph {
public:
    struct Edge { int to; long long weight; };

    static constexpr long long INF = std::numeric_limits<long long>::max();

    explicit Graph(std::size_t n, bool directed = false)
        : n_(n), directed_(directed), adj_(n) {}

    std::size_t size() const { return n_; }
    bool directed() const { return directed_; }

    void add_edge(int u, int v, long long weight = 1) {
        check(u);
        check(v);
        adj_[static_cast<std::size_t>(u)].push_back(Edge{v, weight});
        if (!directed_) adj_[static_cast<std::size_t>(v)].push_back(Edge{u, weight});
        if (weight < 0) has_negative_ = true;
    }

    const std::vector<Edge>& neighbors(int u) const {
        check(u);
        return adj_[static_cast<std::size_t>(u)];
    }

    // BFS 访问顺序
    std::vector<int> bfs_order(int src) const {
        check(src);
        std::vector<int> order;
        std::vector<char> seen(n_, 0);
        std::queue<int> q;
        seen[static_cast<std::size_t>(src)] = 1;
        q.push(src);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            order.push_back(u);
            for (const Edge& e : adj_[static_cast<std::size_t>(u)])
                if (!seen[static_cast<std::size_t>(e.to)]) {
                    seen[static_cast<std::size_t>(e.to)] = 1;
                    q.push(e.to);
                }
        }
        return order;
    }

    // 单源最短路（按边数），不可达为 -1
    std::vector<int> bfs_distances(int src) const {
        check(src);
        std::vector<int> dist(n_, -1);
        std::queue<int> q;
        dist[static_cast<std::size_t>(src)] = 0;
        q.push(src);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (const Edge& e : adj_[static_cast<std::size_t>(u)])
                if (dist[static_cast<std::size_t>(e.to)] == -1) {
                    dist[static_cast<std::size_t>(e.to)] = dist[static_cast<std::size_t>(u)] + 1;
                    q.push(e.to);
                }
        }
        return dist;
    }

    // DFS 访问顺序（迭代实现，按邻接表顺序）
    std::vector<int> dfs_order(int src) const {
        check(src);
        std::vector<int> order;
        std::vector<char> seen(n_, 0);
        dfs_visit(src, seen, order);
        return order;
    }

    // Dijkstra 单源最短路（要求非负权）；不可达为 INF
    std::vector<long long> dijkstra(int src) const {
        check(src);
        if (has_negative_)
            throw std::invalid_argument("ceh::Graph::dijkstra: negative edge weight");
        std::vector<long long> dist(n_, INF);
        using Node = std::pair<long long, int>;   // (距离, 顶点)
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
        dist[static_cast<std::size_t>(src)] = 0;
        pq.push({0, src});
        while (!pq.empty()) {
            auto [d, u] = pq.top(); pq.pop();
            if (d > dist[static_cast<std::size_t>(u)]) continue;
            for (const Edge& e : adj_[static_cast<std::size_t>(u)]) {
                long long nd = d + e.weight;
                if (nd < dist[static_cast<std::size_t>(e.to)]) {
                    dist[static_cast<std::size_t>(e.to)] = nd;
                    pq.push({nd, e.to});
                }
            }
        }
        return dist;
    }

    // 拓扑排序（Kahn）。仅对有向图有意义；存在环时返回空 vector。
    std::vector<int> topological_sort() const {
        std::vector<int> indeg(n_, 0);
        for (std::size_t u = 0; u < n_; ++u)
            for (const Edge& e : adj_[u]) ++indeg[static_cast<std::size_t>(e.to)];
        std::queue<int> q;
        for (std::size_t u = 0; u < n_; ++u)
            if (indeg[u] == 0) q.push(static_cast<int>(u));
        std::vector<int> order;
        while (!q.empty()) {
            int u = q.front(); q.pop();
            order.push_back(u);
            for (const Edge& e : adj_[static_cast<std::size_t>(u)])
                if (--indeg[static_cast<std::size_t>(e.to)] == 0) q.push(e.to);
        }
        if (order.size() != n_) return {};   // 有环
        return order;
    }

    // 无向图连通分量数量
    std::size_t connected_components() const {
        std::vector<char> seen(n_, 0);
        std::size_t count = 0;
        std::vector<int> tmp;
        for (std::size_t s = 0; s < n_; ++s) {
            if (!seen[s]) {
                ++count;
                tmp.clear();
                dfs_visit(static_cast<int>(s), seen, tmp);
            }
        }
        return count;
    }

private:
    std::size_t n_;
    bool directed_;
    std::vector<std::vector<Edge>> adj_;
    bool has_negative_ = false;

    void check(int u) const {
        if (u < 0 || static_cast<std::size_t>(u) >= n_)
            throw std::out_of_range("ceh::Graph: vertex index out of range");
    }

    void dfs_visit(int u, std::vector<char>& seen, std::vector<int>& order) const {
        seen[static_cast<std::size_t>(u)] = 1;
        order.push_back(u);
        for (const Edge& e : adj_[static_cast<std::size_t>(u)])
            if (!seen[static_cast<std::size_t>(e.to)]) dfs_visit(e.to, seen, order);
    }
};

}  // namespace ceh

#endif  // CEH_ALGORITHMS_GRAPH_HPP
