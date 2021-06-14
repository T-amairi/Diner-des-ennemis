/* 
 - Tahar AMAIRI
 - 04/04/2021
 - MAIN3 Polytech Sorbonne
 - Micro projet Algo : [Dîner des ennemis]
*/

/**************** LIB && STRUCT ***************/

#include <iostream>
#include <random>
#include <algorithm>
#include <vector>
#include <map>
#include <string>
#include <time.h>
#include <chrono>

using namespace std;

//invité 
typedef struct guest
{   
    int number; //numéro de l'invité  
    vector<int> ene; //liste des ennemis de cet invité 

} guest;

//noeud de l'arbre d'énumération implicite
typedef struct abr
{   
    int weight; //poids 
    vector<int> table; //table du noeud 
    vector<abr*> under_table; //noeuds fils 
    abr* before_table; //noeud père 
    vector<int> current_guests; //suivi des invités 

} abr;

//structure d'une table, pour la prog dynamique 
typedef struct table
{   
    vector<int> guests; //les invités assis à cette table 
    int size; //la taille de la table 

} table;

/************** PARTIE A && PARTIE D ***********/

//génére de façon aléatoire une liste d'ennemis 
vector<int> rand_ene(int guest_number, int TOTAL_GUESTS)
{
    random_device rd;
    mt19937 re(rd());
    uniform_int_distribution<int> distribution_total_ene(0,TOTAL_GUESTS - 1);
    uniform_int_distribution<int> distribution_ene(1,TOTAL_GUESTS);

    vector<int> list_ene;
    int total_ene = distribution_total_ene(re);
    int ene;

    while(1)
    {
        ene = distribution_ene(re);

        if(ene != guest_number)
        {
            break;
        }
    }

    list_ene.push_back(ene);

    int i = 1;   

    while(i<total_ene)
    {
        ene = distribution_ene(re);

       if(!(find(list_ene.begin(), list_ene.end(), ene) != list_ene.end()) && (ene != guest_number))
       {
            list_ene.push_back(ene);
            i++;
        } 
    }
    
    return list_ene;
}

//initialise une liste de TOTAL_GUESTS invités
vector<guest> ini_guest(int TOTAL_GUESTS, double ratio)
{
    vector<guest> list_guest;
    
    for(int i = 0; i<TOTAL_GUESTS; i++)
    {
        guest temp;
        temp.number = i+1;
        list_guest.push_back(temp);
    }

    random_device rd;
    mt19937 re(rd());
    uniform_int_distribution<int> distribution_guest(1,TOTAL_GUESTS);

    int shuffle;
    int i = 0;
   
    while(i<static_cast <int>(TOTAL_GUESTS*ratio))
    {
        shuffle = distribution_guest(re);

        if(list_guest[shuffle-1].ene.empty())
        {
            list_guest[shuffle-1].ene = rand_ene(shuffle,TOTAL_GUESTS);
            i++;
        }
    }

    int target;

    for(int i = 0; i<TOTAL_GUESTS; i++)
    {
        if(!list_guest[i].ene.empty())
        {
            for(long unsigned int j = 0; j<list_guest[i].ene.size(); j++)
            {
                target = list_guest[i].ene[j];

                if(!(find(list_guest[target-1].ene.begin(), list_guest[target-1].ene.end(), i+1) != list_guest[target-1].ene.end()))
                {
                    list_guest[target-1].ene.push_back(i+1);
                }
            }
        }
    }

    return list_guest;
}

//affiche les invités et leurs ennemis respectifs
void disp_guest(vector<guest> list_guest, int TOTAL_GUESTS)
{
    for(int i = 0; i<TOTAL_GUESTS; i++)
    {
        printf("Invité numéro : %d",list_guest[i].number);
        printf("\nEnnemi(s) :");

        for(long unsigned int j = 0; j<list_guest[i].ene.size(); j++)
        {
            printf("%d ",list_guest[i].ene[j]);
        }

        printf("\n\n");
    }

    return;
}

//vérifie si une combinaison est éligible selon la composition des invités
bool IfEne(vector<guest> guests_list, vector<int> comb)
{
    vector<int> temp;

    for(long unsigned int i = 0; i<comb.size(); i++)
    {

        temp = comb;
        temp.erase(find(temp.begin(),temp.end(),comb[i]));

        for(long unsigned int j = 0; j<temp.size(); j++)
        {
            if(find(guests_list[comb[i]-1].ene.begin(),guests_list[comb[i]-1].ene.end(), temp[j]) != guests_list[comb[i]-1].ene.end())
            {
                return true;
            }
        }
    }

    return false;
}

//permet de stocker dans res les combinaisons éligibles en utilisant un algo de backtracking et en excluant celles qui ne le sont pas
void back_tracking_comb(vector<vector<int>>& res, vector<guest> guests_list, vector<int> guest_numbers, vector<int>& temp, int start_idx)
{
    if(!temp.empty())
    {
        res.push_back(temp);
    }

    vector<int> tmp;

    for(int i = start_idx; i < static_cast<int>(guest_numbers.size()); i++)
    {
        tmp = temp;
        tmp.push_back(guest_numbers[i]);

        if(!IfEne(guests_list,tmp))
        {
            back_tracking_comb(res,guests_list,guest_numbers,tmp,i+1);
        }

        tmp.clear();
    }
}

//retourne toutes les combinaisons éligibles selon la config des invités sous forme d'un vector en utilisant back_tracking_comb()
vector<vector<int>> comb_table(vector<guest> guests_list, int TOTAL_GUESTS)
{
    vector<vector<int>> checked_comb;
    vector<int> guest_numbers;
    vector<int> temp;
    
    for(int j = 0; j<TOTAL_GUESTS; j++)
    {
        guest_numbers.push_back(j+1);
    }

    back_tracking_comb(checked_comb,guests_list,guest_numbers,temp,0);

    sort(checked_comb.begin(), checked_comb.end(), [](const vector<int> & a, const vector<int> & b){ return a.size() > b.size();});
    return checked_comb;
}

//affiche les combinaisons éligibles des tables 
void disp_table(vector<vector<int>> res)
{
    vector<int> temp;

    printf("* Table(s) possible(s) :\n\n");

    for(long unsigned int j = 0; j<res.size(); j++)
    {
        temp = res[j];
        printf("[");

        for(long unsigned int i = 0; i<temp.size(); i++)
        {
            printf(" %d",temp[i]);
        }

        printf(" ]\n");
    }

    printf("\n");
    cout << "Total de table(s) possible(s) (M): " << res.size() <<' ';
    return;
}

/**************** PARTIE B ***************/

//permet de trier dans une map par taille les tables et selon une taille spécifique 
map<int,vector<vector<int>>> sort_by_size(vector<vector<int>> tables, int size_table)
{
    map<int,vector<vector<int>>> res;
    vector<int> temp;
    int size;

    for(long unsigned int j = 0; j<tables.size(); j++)
    {
        temp = tables[j];
        size = static_cast <int>(temp.size());

        if(size_table >= size)
        {
            if(res.find(size) == res.end()) 
            {
                vector<vector<int>> new_vec;
                res.insert({size,new_vec});
                res[size].push_back(temp);
            }  

            else
            {
                res[size].push_back(temp);
            }
        }
    }

    return res;
}

//crée un nouveau noeud pour abr
abr * newNode(vector<int> table, vector<int> current_guests, int weight, abr* before_table)
{
    abr * temp = new abr;
    temp->table = table;
    temp->weight = weight;
    temp->before_table = before_table;
    temp->current_guests = current_guests;
    return temp;
}
 
//affiche les informations d'un noeud 
void disp_node(abr * node)
{
    printf("Noeud actuel :\n");
    printf("[ ");
    for(auto it = node->table.begin(); it != node->table.end(); it++)
    {
        cout << *it << ' ';
    }
    printf("]\n");

    printf("Poids du noeud actuel: %d\n",node->weight);

    printf("Noeud père :\n");
    printf("[ ");
    if(node->before_table != NULL)
    {
        for(auto it = node->before_table->table.begin(); it != node->before_table->table.end(); it++)
        {
            cout << *it << ' ';
        }
    }
    printf("]\n");
    
    printf("Noeud(s) fils :\n");
    for(long unsigned int j = 0; j<node->under_table.size(); j++)
    { 
        printf("[ ");
        for(auto it = node->under_table[j]->table.begin(); it != node->under_table[j]->table.end(); it++)
        {
            cout << *it << ' ';
        }

        printf("]\n");
    }

    printf("Evolution du BnB pour ce noeud:\n");
    printf("[ ");
    for(auto it = node->current_guests.begin(); it != node->current_guests.end(); it++)
    {
        cout << *it << ' ';
    }
    printf("]\n\n");

    return;
}

//free l'arbre pour l'énumération 
void free_abr(abr *root)
{
    for(long unsigned int j = 0; j<root->under_table.size(); j++)
    { 
       free_abr(root->under_table[j]);
    }

    delete root;
    return;
}

//fonction récursive énumérant et testant toutes les combinaisons des tables
void recur_enum(abr *node, vector<vector<int>> tables, vector<int> all_guests)
{   
    vector<int> diff;
    set_difference(all_guests.begin(), all_guests.end(),(node->current_guests).begin(), 
    (node->current_guests).end(), inserter(diff, diff.begin()));

    int size = static_cast <int>(diff.size());

    if(size == 0)
    {   
        return;
    }

    auto it_vect = find(tables.begin(), tables.end(), node->table);

    if(it_vect != tables.end())
    {
        tables.erase(it_vect);
    }
    
    map<int,vector<vector<int>>> under_tables = sort_by_size(tables, static_cast <int>((node->table).size()));

    for (auto it = under_tables.begin(), next_it = it; it != under_tables.end(); it = next_it)
    {
        ++next_it;
  
        if(it->first>size)
        {
            under_tables.erase(it);
        }

        else
        {
            for(long unsigned int k = 0; k<it->second.size(); k++)
            {
                for(long unsigned int j = 0; j<it->second[k].size(); j++)
                {
                    if(!(count(diff.begin(), diff.end(),it->second[k][j])))
                    {
                        it->second[k].clear();
                        break;
                    }
                }
            }

            if(it->second.empty())
            {
               under_tables.erase(it); 
            }   
        }
    }

    if(under_tables.empty())
    {
        return;
    }

    vector<int> table;
    vector<int> current_guests;
    
    for (auto it = under_tables.begin(); it != under_tables.end(); it++)
    {
        for(long unsigned int i = 0; i<it->second.size(); i++)
        {
            if(!it->second[i].empty())
            {
                table = it->second[i];
                merge(table.begin(),table.end(),(node->current_guests).begin(),(node->current_guests).end(),
                back_inserter(current_guests));
                
                (node->under_table).push_back(newNode(table,current_guests,(node->weight)+1,node));
                current_guests.clear();
            }
        }
    }

    for(long unsigned int i = 0; i<(node->under_table).size(); i++)
    {
        recur_enum((node->under_table)[i],tables,all_guests);
    }

    return;
}

//permet d'obtenir toutes les feuilles de l'abr construit suite à l'énumération
void get_last_nodes(abr * root, vector<abr*> &last_nodes, int TOTAL_GUESTS)
{
    for(long unsigned int i = 0; i<(root->under_table).size(); i++)
    {
        get_last_nodes((root->under_table)[i],last_nodes,TOTAL_GUESTS);
    }

    if((static_cast<int>(root->current_guests.size()) == TOTAL_GUESTS) && (root->table[0] != 0))
    {
        last_nodes.push_back(root);
        return;
    }

    return;
}

//affiche la meilleure combinaison de l'abr donc la composition de table minimale  
void disp_best_node(vector<abr*> &last_nodes)
{
    if(last_nodes.empty())
    {
        printf("Chacun des invités doit être seul !\n");
        return;
    }
    
    int min_weight = RAND_MAX;
    vector<int> temp;
    abr* best_node;

    for(long unsigned int j = 0; j<last_nodes.size(); j++)
    {         
        if(min_weight>last_nodes[j]->weight)
        {
            min_weight = last_nodes[j]->weight;
            best_node = last_nodes[j];
        }
    }

    printf("%d table(s) requise(s) :\n",best_node->weight);
    while(best_node->before_table != NULL)
    {
        temp = best_node->table;
        printf("[");

        for(long unsigned int i = 0; i<temp.size(); i++)
        {
            printf(" %d",temp[i]);
        }

        printf(" ]\n");

        best_node = best_node->before_table;
    }

    return;
}

//initialise l'énumération implicite 
void enum_brute(vector<vector<int>> tables,int TOTAL_GUESTS)
{
    vector<int> all_guests;
    vector<int> temp;
    
    for(int j = 0; j<TOTAL_GUESTS; j++)
    {
        all_guests.push_back(j+1);
        temp.push_back(0);
    }

    abr* root = newNode(temp,temp,0,NULL);
    temp.clear();
   
    for(long unsigned int j = 0; j<tables.size() - TOTAL_GUESTS; j++)
    {
        temp = tables[j];
        (root->under_table).push_back(newNode(temp,temp,1,root));
    }

    clock_t start, end;
    start = clock();

    for(long unsigned int i = 0; i<(root->under_table).size(); i++)
    {
        recur_enum((root->under_table)[i],tables,all_guests);
    }
    
    vector<abr*> last_nodes;
    get_last_nodes(root,last_nodes,TOTAL_GUESTS);
    disp_best_node(last_nodes);
    free_abr(root);
    end = clock();

    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    printf("\n");
    cout << "Temps d'exécution : " << fixed << time_taken << " sec " << endl;
    
    return;
}

/**************** PARTIE C ***************/

//génére une liste de la struct table
vector<table> ini_table(vector<vector<int>> list_tables)
{
    vector<table> res;
    
    for(long unsigned int j = 0; j<list_tables.size(); j++)
    {
        table temp;
        temp.guests = list_tables[j];
        temp.size = static_cast<int>(list_tables[j].size());
        res.push_back(temp);
    }

    reverse(res.begin(), res.end());

    return res;
}

//vérifie si g1 et g2 ont un élément commun  
int IfDuplicates(vector<int> g1,vector<int> g2)
{   
    for(long unsigned int j = 0; j<g1.size(); j++)
    {
       if(count(g2.begin(), g2.end(),g1[j]))
        {
            return 1;
        }
    }

    return 0;
}

//permet d'obtenir et filtrer les compositions en lisant la matrice fourni par la prog dynamique 
vector<int> get_best_comp(vector<vector<int>> DP, vector<table> list_tables, int TOTAL_GUESTS)
{
    vector<int> res;
    vector<int> res_cache;
    int cache;
    int size = static_cast<int>(DP.size());
    int sum;
    int test;

    for(int i = size - 1; i >= TOTAL_GUESTS - 1; --i)
    {   
        res.push_back(i);
        cache = i;
        sum = list_tables[i].size;

        for(int j = size - 1; j >= 0; --j)
        { 
            if(i != size - 1)
            {
                if(sum == TOTAL_GUESTS && res_cache.size() > res.size())
                {   
                    res_cache = res;
                    break;
                }

                else if(res_cache.size() < res.size())
                {
                    break;
                }

                else if(DP[i][j] == 0 && DP[j][cache] == 0){
                    
                    test = 0;

                    for(long unsigned int k = 0; k < res.size()-1; k++)
                    {
                        if(DP[j][res[k]] == 1)
                        {
                            test = 1;
                            break;
                        }
                    }

                    if(!test)
                    {
                        cache = j;
                        res.push_back(j);
                        sum = sum + list_tables[j].size;
                    }
                }
            }

            else
            {  
                if(DP[i][j] == 0 && DP[j][cache] == 0)
                {
                    test = 0;

                    for(long unsigned int k = 0; k < res.size()-1; k++)
                    {
                        if(DP[j][res[k]] == 1)
                        {
                            test = 1;
                            break;
                        }
                    }
                    
                    if(!test)
                    {
                        cache = j;
                        res.push_back(j);
                        sum = sum + list_tables[j].size;
                    }
                } 

                if(sum == TOTAL_GUESTS)
                {
                    res_cache = res;
                    break;
                }
            }
        }

        res.clear();
    }

    return res_cache;
}

//prog dynamique pour résoudre les différentes combinaisons 
vector<vector<int>> dynamic_prog(vector<table> list_tables, int TOTAL_GUESTS)
{
    int size = static_cast<int>(list_tables.size());
    vector<vector<int>> DP(size,vector<int>(size,2));

    for(int i = 0; i<size; ++i)
    {
        DP[i][i] = 1;
    }

    for(int i = 0; i < size; ++i)
    {
        for(int j = 0; j < size; ++j)
        {
            if(DP[i][j] == 2)
            {
                if(DP[j][i] != 2)
                {
                    DP[i][j] = DP[j][i];
                }

                else
                {
                    if(list_tables[i].size + list_tables[j].size <= TOTAL_GUESTS)
                    {
                        DP[i][j] = IfDuplicates(list_tables[i].guests,list_tables[j].guests);
                    }

                    else
                    {
                        DP[i][j] = 1;
                    }
                }
            }
        }
    }

    return DP;
}

//permet d'afficher la composition de table minimale 
void disp_best_comp(vector<vector<int>> list_tables, int TOTAL_GUESTS)
{
    vector<table> vect_tables = ini_table(list_tables);
   
    clock_t start, end;
    start = clock();
    auto DP = dynamic_prog(vect_tables,TOTAL_GUESTS);
    auto res = get_best_comp(DP,vect_tables,TOTAL_GUESTS);
    
    if(res.empty())
    {
        printf("Chacun des invités doit être seul !\n");
        end = clock();

        double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
        printf("\n");
        cout << "Temps d'exécution : " << fixed << time_taken << " sec " << endl;
        return;
    }

    vector<int> temp;

    printf("%d Table(s) requise(s) :\n",static_cast<int> (res.size()));

    for(long unsigned int j = 0; j<res.size(); j++)
    {
        temp = vect_tables[res[j]].guests;
        printf("[");

        for(long unsigned int i = 0; i<temp.size(); i++)
        {
            printf(" %d",temp[i]);
        }

        printf(" ]\n");
    }

    end = clock();

    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    printf("\n");
    cout << "Temps d'exécution : " << fixed << time_taken << " sec " << endl;

    return;
}

int main(){

    //nombre d'invités
    int TOTAL_GUESTS ;
    //nombre d'invité initialisé avec des ennemis
    double ratio;

    //cin utilisateur
    cout << "Nombre d'invités strictement positive (n): ";
	cin >> TOTAL_GUESTS;

    //check
    if(TOTAL_GUESTS <= 0)
    {
        printf("Mauvaise valeur pour le nombre d'invités");
        return 0;
    }

    //cin utilisateur
    cout << "Ratio d'invités initialisés avec des ennemis (compris entre [0,1]): ";
    cin >> ratio;

    //check
    if(ratio < 0 || ratio > 1)
    {
        printf("Mauvaise valeur pour ratio");
        return 0;
    }

    //initialisation des instances
    printf("* La composition des invités :\n\n");
    auto guests_list = ini_guest(TOTAL_GUESTS,ratio);
    disp_guest(guests_list,TOTAL_GUESTS);
    clock_t start, end;
    start = clock();
    auto comb_tables = comb_table(guests_list,TOTAL_GUESTS);
    end = clock();
    disp_table(comb_tables);
    double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
    printf("\n");
    cout << "Temps d'exécution : " << fixed << time_taken << " sec " << endl;
    
    //prog dynamique
    printf("\n");
    printf("* Programmation dynamique :\n\n");
    disp_best_comp(comb_tables,TOTAL_GUESTS);

    //énumération brute 
    printf("\n");
    printf("* Enumération brute :\n\n");
    enum_brute(comb_tables,TOTAL_GUESTS);

    return 0;
}