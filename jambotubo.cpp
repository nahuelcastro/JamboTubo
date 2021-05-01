#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <set>
#include <map>

using namespace std;

int INFTY = 10e6; // Valor para indicar que no hubo solución.
int n, R;
vector<int> w, r; // w = pesos, r = resistencias
vector<bool> agregados;
int solucion_actual = 0;

// h: hasta que producto veo si se rompio alguna resistencia
bool rompeResistencia(int h){   // O(2n) = O(n)
    int peso_total = 0;
    vector<int> rj; // resistencias de los productos dentro del jambotubo
    vector<int> wj; // pesos parciales de los productos dentro del jambotubo

    for (int i = 0; i < h ; ++i) { // O(n)
        if (agregados[i]) {
            peso_total += w[i];
            rj.push_back(r[i]);
            wj.push_back(peso_total);
        }
    }

    // ver que ninguno se rompio, evaluando (resistencia < peso final - peso parcial hasta ese producto) = r < peso que tiene encima?
    for (int i = 0; i < rj.size() ; ++i) { // O(n)
        if (rj[i] < peso_total - wj[i]) return true;
    }

    return false;
}


// i: posicion del producto a considerar en este nodo.
// k: cantidad de productos agregados hasta este nodo.
// t: suma de los pesos de los productos seleccionados hasta este nodo.
void FB(int i = 0,int k = 0, int t = 0){ //O(n * 2^n )

    if (i == n){
        if (t <= R && !rompeResistencia(n)){                         // O(n)
            solucion_actual = max(solucion_actual, k);
        }
    } else {                                                         
        agregados[i] = true;
        FB(i + 1, k+1, t + w[i]);
        agregados[i] = false;
        FB(i + 1, k, t);
    }
}



bool poda_factibilidad = true; // define si la poda por factibilidad esta habilitada.
bool poda_optimalidad = true; // define si la poda por optimalidad esta habilitada.

// i: posicion del producto a considerar en este nodo.
// k: cantidad de productos agregados hasta este nodo.
// t: suma de los pesos de los productos seleccionados hasta este nodo.
void BT(int i = 0, int k = 0, int t = 0){ // O(n^2 * 2^n) 

    if (poda_optimalidad && k + n - i <= solucion_actual) return;    // O(1) 
    if (poda_factibilidad && (t > R || rompeResistencia(i))) return; // O(n)

    if (i == n){
        if (t <= R && !rompeResistencia(i)){                         // O(n)
            solucion_actual = max(solucion_actual, k);
        }
    } else {                                                         
        agregados[i] = true;
        BT(i + 1, k + 1, t + w[i]);
        agregados[i] = false;
        BT(i + 1, k, t);
    }
}


vector<vector<int>> M; // Memoria de PD.
const int UNDEFINED = -1;

// i: posicion del producto a considerar en este nodo.
// t: suma de los pesos de los productos seleccionados hasta este nodo.
// rp: resistencia parcial.
int PD(int i = 0, int t = 0, int rp = R){
    
    if (t > R || rp < 0) return -1;
    if (i == n) return 0;

    if(M[i][rp] == UNDEFINED){
        M[i][rp] = max(PD(i+1, t, rp), 1 + PD(i+1, t+w[i], min(r[i], rp-w[i])));
    }

    return M[i][rp];
}


// Recibe por parámetro qué algoritmos utilizar para la ejecución separados por espacios.
// Imprime por clog la información de ejecución de los algoritmos.
// Imprime por cout el resultado de algun algoritmo ejecutado.
int main(int argc, char** argv)
{
    // Leemos el parametro que indica el algoritmo a ejecutar.
    map<string, string> algoritmos_implementados = {
            {"FB", "Fuerza Bruta"}, {"BT", "Backtracking con podas"}, {"BT-F", "Backtracking con poda por factibilidad"},
            {"BT-O", "Backtracking con poda por optimalidad"}, {"DP", "Programacion dinámica"}
    };

    // Verificar que el algoritmo pedido exista.
    if (argc < 2 || algoritmos_implementados.find(argv[1]) == algoritmos_implementados.end())
    {
        cerr << "Algoritmo no encontrado: " << argv[1] << endl;
        cerr << "Los algoritmos existentes son: " << endl;
        for (auto& alg_desc: algoritmos_implementados) cerr << "\t- " << alg_desc.first << ": " << alg_desc.second << endl;
        return 0;
    }
    string algoritmo = argv[1];

    // Leemos el input.
    cin >> n >> R;
    w.assign(n, 0);
    r.assign(n, 0);
    agregados.assign(n,false);
    for (int i = 0; i < n; ++i) cin >> w[i] >> r[i];

    // Ejecutamos el algoritmo y obtenemos su tiempo de ejecución.
    int optimum;
    optimum = INFTY;
    auto start = chrono::steady_clock::now();
    if (algoritmo == "FB")
    {
        FB();
        optimum = solucion_actual;
    }
    else if (algoritmo == "BT")
    {
        poda_optimalidad = poda_factibilidad = true;
        BT();
        optimum = solucion_actual;
    }
    else if (algoritmo == "BT-F")
    {
        poda_optimalidad = false;
        poda_factibilidad = true;
        BT();
        optimum = solucion_actual;
    }
    else if (algoritmo == "BT-O")
    {
        poda_optimalidad = true;
        poda_factibilidad = false;
        BT();
        optimum = solucion_actual;
    }
    else if (algoritmo == "DP")
    {
        // Precomputamos la solucion para los estados.
        M = vector<vector<int>>(n+1, vector<int>(R+1, UNDEFINED));
        for (int i = 0; i < n+1; ++i)
            for (int j = 0; j < R+1; ++j)
                PD(i, j, R);

        // Obtenemos la solucion optima.
        optimum = PD(0, 0, R);
    }
    auto end = chrono::steady_clock::now();
    double total_time = chrono::duration<double, milli>(end - start).count();

    // Imprimimos el tiempo de ejecución por stderr.
    clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << (optimum == INFTY ? -1 : optimum) << endl;
    return 0;
}
