#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cstring>
#include <queue>

#define INF 0x3f3f3f3f

using namespace std;

/**
 * @brief Add a node to a bipartite graph using vector element from STL
 * 
 * @param graph (vector<vector<int>>)
 * @param user (int)
 * @param job (int)
 */
void addNode(vector<vector<int>>& graph, int user, int job){
  if(graph.size() == user){
    vector<int> vec;
    vec.push_back(job);
    graph.push_back(vec);
    return;
  }
  graph[user].push_back(job);
}

/**
 * @brief Print the graph using the int-user map and the int-job map
 * 
 * @param graph (vector<vector<int>>)
 * @param userMap (vector<string>)
 * @param jobMap (vector<string>)
 */
void printGraph(vector<vector<int>> graph, vector<string> userMap, vector<string> jobMap){
  for(long unsigned int j = 0; j<graph.size(); j++){
    cout<< userMap[j]<<" ----> ";
    for(long unsigned int k = 0; k<graph[j].size(); k++){
      cout<<jobMap[graph[j][k]]<<' ';
    }
    cout<<'\n';
  }
}

/**
 * @brief Search the string value in the vector vec, if not present add the value, returns the position of the value
 * 
 * @param vec vector<string>
 * @param value string
 * @return int 
 */
int searchVector(vector<string>& vec, string value){
  int local;
  local = vec.size();
  for(int k = 0; k < vec.size(); k++){
    if(vec[k] == value){
      local = k;
      break;
    }
  }
  if(local == vec.size()){
    vec.push_back(value);
  }
  return local;
}

/**
 * @brief DFS algorithm that returns true if a matching is possible to user
 * 
 * @param graph vector<vector<int>>
 * @param user int
 * @param seen bool[]
 * @param matchs int[]
 * @return true 
 * @return false 
 */
bool bipartiteMatch(vector<vector<int>> graph, int user, bool (&seen)[], int (&matchs)[]){
  vector<int> jobs = graph[user];
  for(int i = 0; i<jobs.size(); i++){
    if(!seen[jobs[i]]){
      seen[jobs[i]] = true;
      if(matchs[jobs[i]] < 0 || bipartiteMatch(graph, matchs[jobs[i]], seen, matchs)){
        matchs[jobs[i]] = user;
        return true;
      }
    }
  }
  return false;
}

/**
 * @brief Maximum flow algorithm or Ford-Fukelson algorithm to find maximum bipartite matching, returns the maximum number of matchings possible
 * 
 * @param graph vector<vector<int>>
 * @param numUsers int
 * @param numJobs int
 * @return int 
 */
int maxFlow(vector<vector<int>> graph, int numUsers, int numJobs){
  int matchs[numJobs], ans = 0;
  memset(matchs, -1, sizeof(matchs));
  for(int i = 0; i< numUsers; i++){
    bool seen[numJobs];
    memset(seen, 0, sizeof(seen));
    if(bipartiteMatch(graph, i, seen, matchs))
      ans++;
  }
  return ans;
}
/**
 * @brief Auxiliar DFS function from the greedy algorithm, returns true if a match is possible, false otherwise
 * 
 * @param graph vector<vector<int>>
 * @param user int
 * @param matchU int[]
 * @param matchJ int[]
 * @param dist int[]
 * @return true 
 * @return false 
 */
bool dfs(vector<vector<int>> graph, int user, int (&matchU)[], int (&matchJ)[], int (&dist)[]){
  if(user != 0){
    vector<int>::iterator itr;
    for(itr = graph[user-1].begin(); itr!= graph[user-1].end(); ++itr){
      int job = *itr + 1; // Get the job ID 
      if(dist[matchJ[job]] == dist[user]+1){
        if(dfs(graph, matchJ[job], matchU, matchJ, dist)){
          matchJ[job] = user;
          matchU[user] = job;
          return true;
        }
      }
    }
    //if no augmenting path begining with user exists
    dist[user] = INF;
    return false;
  }
  // if there is an augmenting path begining with user
  return true;
}

/**
 * @brief Auxiliar BFS algorithm for the greedy implementation, returns true if an augmentation path is present
 * 
 * @param graph vector<vector<int>>
 * @param matchsU int[]
 * @param users int
 * @param matchsJ int[]
 * @param jobs int
 * @param dist int[]
 * @return true 
 * @return false 
 */
bool bfs(vector<vector<int>> graph, int (&matchsU)[], int users, int (&matchsJ)[], int jobs, int (&dist)[]){
  queue<int> qu;
  //set distances as 0 if user is not matching
  for(int u = 1; u<= users; u++){
    if(matchsU[u] == 0){
      dist[u] = 0;
      qu.push(u);
      break;
    }
    dist[u] = INF;
  }
  dist[0] = INF;
  //while still exists user without jobs
  while(!qu.empty()){
    int u = qu.front();
    qu.pop();
    if(dist[u]<dist[0]){
      vector<int>::iterator it;
      //the user i in the array matchsU, is equivalent to the user i-1 in the graph
      //the job j in the array matchsJ, is equivalent to the job j-1 in the graph
      for(it = graph[u-1].begin();it!= graph[u-1].end(); ++it){
        int job = *it + 1;
        if(dist[matchsJ[job]] == INF){
          dist[matchsJ[job]] = dist[u] +1;
          qu.push(matchsJ[job]);
        }
      }
    }
  }
  //if is possible to return to the dummy vertice then there is an augmentint path
  return (dist[0] != INF);
}

/**
 * @brief Greedy algorithm (Hopcroft-Karp) to find sub-optimal solution, returns in the best case the maximum bipartite matching, but with time efficiency.
 * 
 * @param graph vector<vector<int>>
 * @param users int
 * @param jobs int
 * @return int 
 */
int hopcroftGreed(vector<vector<int>> graph, int users, int jobs){
  // i have to add 1 to the number of users and jobs in order to accomodate the dummy vertice, that is connected to all jobs and users in the graph
  int matchsU[users+1];
  int matchsJ[jobs+1];
  int dist[users+1];
  memset(matchsU, 0, sizeof(matchsU));
  memset(matchsJ, 0, sizeof(matchsJ));
  int ans = 0;
  //while augmentation paths exist
  while(bfs(graph,matchsU, users, matchsJ, jobs, dist)){
    for(int i = 1; i<=users; i++){
      //i make a depth search and remove previously connected vertices and add the new ones (if possible)
      if(matchsU[i]==0 && dfs(graph,i,matchsU,matchsJ,dist))
        ans++;
    }
  }
  return ans;
}

/**
 * @brief Main program implements greedy and max flux algorithms to determine the maximum bipartite matching
 * 
 * @param argc int
 * @param argv char[]
 * @return int 
 */
int main(int argc, char const *argv[]) {
  // every job and user will gain an int as id
  int users, jobs, matchs, userNum = 0, jobNum = 0;
  cin>>users>>jobs>>matchs;
  vector<vector<int>> graph;
  vector<string> userMap;
  vector<string> jobMap;
  string user, job;
  for(int i = 0; i<matchs; i++){
    cin>>user>>job;
    //Search the string value in the vector vec, if not present add the value, returns the position of the value
    userNum = searchVector(userMap, user);
    jobNum = searchVector(jobMap, job);
    addNode(graph, userNum, jobNum);
  }
  // printGraph(graph, userMap, jobMap);
  int greedMatch = hopcroftGreed(graph,users,jobs);
  int bestMatch = maxFlow(graph, users, jobs);
  cout<<"Guloso: "<<greedMatch<<'\n';
  cout<<"Exato: "<<bestMatch<<'\n';
  return 0;
}
