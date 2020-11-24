//问题描述
//输入：一个自然数n
//输出：所有的自然数i, j, k, l 使得n = i ^ 2 + j ^ 2 + k ^ 2 + l ^ 2
//相同数据的组合算同一种。
//每个组合用圆括号括起来，用逗号隔开
//例
//输入：65536
//输出：(0,0,0,256), (128,128,128,128)
//
#include <iostream>
#include <chrono>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <compare>
struct r {//存放结果的数据结构
	std::array<int, 4> c;
	r (int i, int j, int k, int l) : c{i, j, k, l} {
		std::sort(c.begin(), c.end());
	}
	bool operator==(const r& other) const {
		return c[0] == other.c[0] && c[1] == other.c[1] && c[2] == other.c[2] && c[3] == other.c[3];
	}
};
struct rHash {//r的哈希，将低16位拼成long之后哈希
	size_t operator() (const r& rr) const {
		long long t =  static_cast<long long>(rr.c[0]) +  static_cast<long long>(rr.c[1]) << 16 +  static_cast<long long>(rr.c[2]) << 32 + static_cast<long long>(rr.c[3]) << 48;
		return std::hash<long long>()(t);
	}
};

using RT = std::unordered_set<r, rHash>;
RT FL_ML(const int);//蛮力算法，O(n^2)时间，O(1)空间
RT FL_HS(const int);//回溯算法，O(n^2)时间，O(1)空间
RT FL_FT(const int);//查找表，平均O(n)时间，最坏O(n^2)，O(n)空间
RT FL_DF(const int);//双向查找算法，O(nlogn)时间，O(n)空间
int sqrti(const int);//整数平方根，向上取整
void printRT(const RT& rt) {//输出结果函数
	for (const auto& rr : rt) {
		std::cout << '(' << rr.c[0] << ',' << rr.c[1] << ','<< rr.c[2] << ','<< rr.c[3] << "), ";
	}
	std::cout << std::endl;
}
RT perf(std::function<RT(const int)> f, const int n) {//计时
	auto start = std::chrono::system_clock::now();
	auto rr = f(n);
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> diff = end - start;
	std::cout << "花费时间（秒）：" << diff.count() << std::endl;
	return rr;
}
void perf_all(const int n) {//放一起的计时
	RT rs[4];
	std::cout << "测试蛮力算法。。。" << std::flush;
	rs[0] = perf(FL_ML, n);
	std::cout << "测试回溯算法。。。" << std::flush;
	rs[1] = perf(FL_HS, n);
	std::cout << "测试查找算法。。。" << std::flush;
	rs[2] = perf(FL_FT, n);
	std::cout << "测试双向查找算法。。。" << std::flush;
	rs[3] = perf(FL_DF, n);
	//检测正确性
	if (rs[0].size() == rs[1].size() && rs[2].size() == rs[3].size() && rs[2].size() == rs[1].size()) {
		std::cout << "Final result: " << std::endl;
		printRT(rs[0]);
	}
	else {
		for (const auto& rr: rs[3]) {rs[0].erase(rr);}
		/*for(const auto& rr : result)*/ { printRT(rs[0]); }
	}
}
int main() {
	int n;
	std::cout << "What's the number?";
	std::cin >> n;
	perf_all(n);
	return 0;
}
int sqrti(const int x) {
	auto x1 = x - 1;
	int s = 1, g0, g1;
	//根据范围猜初值
	if (x1 > 65535) { s += 8; x1 >>= 16;} 
	if (x1 > 255)   { s += 4; x1 >>= 8; }
	if (x1 > 15)    { s += 2; x1 >>= 4; }
	if (x1 > 3)     { s += 1; x1 >>= 2; }
	//牛顿迭代
	g0 = 1 << s;
	g1 = (g0 + (x >> s)) >> 1;
	while(g1 < g0) {
		g0 = g1;
		g1 = (g0 + x/g0) >> 1;
	}
	if (g0 * g0 <= x) { g0 += 1; }
	return g0;
}
RT FL_ML(const int n) {
	RT result;
	auto nm = sqrti(n);
	//四重循环遍历
	//注意对称性，这里取l<=k<=j<=i<nm
	for(auto i = 0; i < nm; i++) {
		auto i2 = i * i;
		for(auto j = 0; j <= i; j++) {
			auto ij2 = i2 + j * j;
			for(auto k = 0; k <= j; k++) {
				auto ijk2 = ij2 + k * k;
				for(auto l = 0; l <= k; l++) {
					if(ijk2 + l * l == n) {
						result.insert(r(i, j, k, l));
						break;
					}
				}
			}
		}
	}
	return result;
}
RT FL_HS(const int n) {
	RT result;
	auto nm = sqrti(n);
	//改进后的四重循环
	//在和超过后n退出
	for(auto i = 0; i < nm; i++) {
		auto i2 = i * i;
		for(auto j = 0; j <= i; j++) {
			auto ij2 = i2 + j * j;
			if (ij2 > n) { break; }
			for(auto k = 0; k <= j; k++) {
				auto ijk2 = ij2 + k * k;
				if (ijk2 > n) { break; }
				for(auto l = 0; l <= k; l++) {
					if(ijk2 + l * l == n) {
						result.insert(r(i, j, k, l));
						break;
					}
				}
			}
		}
	}
	return result;
}
RT FL_FT(const int n) {
	//将问题分解为 n = (i ^ 2 + j ^ 2) + (k ^ 2 + l ^ 2)
	//之后退化为两数和问题
	std::unordered_multimap<int, std::pair<int, int>> ft;
	RT result;
	auto nm = sqrti(n);
	//构造i ^ 2 + j ^ 2的表 
	//i = O(√n), j = O(√n)
	//表规模为O(n)
	for(auto i = 0; i < nm; i++) {
		auto i2 = i * i;
		for(auto j = 0; j <= i; j++) {
			auto ij2 = i2 + j * j;
			if (ij2 <= n) {
				ft.insert({ ij2, {i, j} });
			}
			else { break; }
		}
	}
	for(auto i = 0; i < ft.bucket_count(); i++) {
		for(auto j = ft.begin(i); j != ft.end(i); j++) {
			auto nr = n - j->first;
			//unordered_multimap内部使用桶，平均O(1)，最坏O(n)找到答案
			//每个平方和的解的数量为O(1)
			for (auto it = ft.find(nr);it != ft.end() && it->first == nr; it++) {
				result.insert(r(j->second.first, j->second.second, it->second.first, it->second.second));
			}
		}
	}
	return result;

}
struct DF {
	int i, j, ij2;
	DF(int i, int j) : i(i), j(j), ij2(i * i + j * j) { };
	std::strong_ordering operator <=> (const DF& other) const { return ij2 <=> other.ij2; }
	std::strong_ordering operator <=> (const int other) const { return ij2 <=> other; }
	int operator + (const DF& other) const { return ij2 + other.ij2; }
};
RT FL_DF(const int n) {
	//将问题分解为 n = (i ^ 2 + j ^ 2) + (k ^ 2 + l ^ 2)
	//之后退化为两数和问题
	//构造i ^ 2 + j ^ 2的表 
	//i = O(√n), j = O(√n)
	//表规模为O(n)
	//需要保存平方和的解
	RT result;
	auto nm = sqrti(n);
	auto s = std::vector<std::vector<DF>>();
	s.reserve(nm);
	for(auto i = 0; i < nm; i++) {
		std::vector<DF> t;
		for(auto j = 0; j <= i; j++) {
			DF df(i, j);
			if(df <= n) {
				t.push_back(df);
			}
			else { break; }
		}
		s.push_back(t);
	}
	//对数据排序
	//循环的对最小的区间归并排序
	//每个子向量内数据分别是有序的
	//采用最小堆决定归并的向量
	//时间复杂度为O(nlogn)
	auto comp = [](const std::vector<DF> &a, const std::vector<DF> &b) { return a.size() > b.size();};
	std::make_heap(s.begin(), s.end(), comp);
	auto size = s.size();
	auto push = [&](){ std::push_heap(s.begin(), s.begin() + size, comp); size +=1; };
	auto pop = [&](){ std::pop_heap(s.begin(), s.begin() + size, comp); size -= 1; };
	std::vector<DF> t;
	while(s.size() > 1) {
		pop(); pop();
		auto& s1 = s.back();
		auto& s2 = *(s.end() - 2);
		t.clear();
		t.reserve(s1.size() + s2.size());
		std::merge(s1.begin(), s1.end(), s2.begin(), s2.end(), std::back_inserter(t));
		std::swap(s2, t);
		push();
		s.pop_back();
	}
	//双向查找
	const auto& ft = s[0];
	auto rit = ft.end() - 1;
	//中点之后均为重复的
	for(auto it = ft.begin(); it <= rit; it++) {
		//找到首个和
		while (*rit + *it > n) { rit--; }
		for (auto rt = rit; *it + *rt == n; rt--) {
			result.insert(r(it->i, it->j, rt->i, rt->j));
		}
	}
	return result;
}
