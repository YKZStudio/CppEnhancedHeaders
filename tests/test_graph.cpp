// graph 算法单元测试（零依赖，基于 assert）
#include <ceh/algorithms/graph.hpp>

#include <iostream>
#include <vector>

using ceh::Graph;

static int g_checks = 0;

#define CHECK_TRUE(expr)                                               \
    do { ++g_checks; if (!(expr)) {                                    \
        std::cerr << "FAIL [" << __LINE__ << "] " #expr "\n"; std::exit(1);} } while (0)

#define CHECK_THROWS(ExType, expr)                                     \
    do { ++g_checks; bool t = false; try { (void)(expr); }            \
        catch (const ExType&) { t = true; }                           \
        if (!t) { std::cerr << "FAIL [" << __LINE__ << "] expected throw\n"; std::exit(1);} } while (0)

using IV = std::vector<int>;
using LV = std::vector<long long>;

int main() {
    // 无向路径图 0-1-2-3
    {
        Graph g(4);
        g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 3);
        CHECK_TRUE(g.bfs_order(0) == (IV{0, 1, 2, 3}));
        CHECK_TRUE(g.bfs_distances(0) == (IV{0, 1, 2, 3}));
        CHECK_TRUE(g.dfs_order(0) == (IV{0, 1, 2, 3}));
        CHECK_TRUE(g.connected_components() == 1);
    }

    // 不连通图
    {
        Graph g(5);
        g.add_edge(0, 1); g.add_edge(2, 3);
        CHECK_TRUE(g.connected_components() == 3);          // {0,1} {2,3} {4}
        CHECK_TRUE(g.bfs_distances(0) == (IV{0, 1, -1, -1, -1}));
    }

    // Dijkstra（有向加权）
    {
        Graph g(4, true);
        g.add_edge(0, 1, 1); g.add_edge(0, 2, 4);
        g.add_edge(1, 2, 2); g.add_edge(2, 3, 1);
        CHECK_TRUE(g.dijkstra(0) == (LV{0, 1, 3, 4}));
        // 顶点 3 到别处不可达
        CHECK_TRUE(g.dijkstra(3)[0] == Graph::INF);
    }

    // 拓扑排序（DAG）
    {
        Graph g(4, true);
        g.add_edge(0, 1); g.add_edge(0, 2); g.add_edge(1, 3); g.add_edge(2, 3);
        CHECK_TRUE(g.topological_sort() == (IV{0, 1, 2, 3}));
    }
    // 有环 -> 空
    {
        Graph g(3, true);
        g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 0);
        CHECK_TRUE(g.topological_sort().empty());
    }

    // 越界
    {
        Graph g(3);
        CHECK_THROWS(std::out_of_range, g.add_edge(0, 5));
        CHECK_THROWS(std::out_of_range, g.bfs_order(3));
    }

    // 负权 Dijkstra 抛异常
    {
        Graph g(2, true);
        g.add_edge(0, 1, -2);
        CHECK_THROWS(std::invalid_argument, g.dijkstra(0));
    }

    std::cout << "All graph tests passed (" << g_checks << " checks).\n";
    return 0;
}
