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

// Información de la instancia a resolver.
int n, R;
vector<int> w, r; // w = pesos, r = resistencias

int solucion_actual = 0;
vector<bool> agregados;
vector<int> pesoAcumulado(n, 0);

void agregarPeso(vector<bool> agregados, int i){ // O(n)
    for (int j = 0; j < n ; j++) {
        if (agregados[j]) {
            pesoAcumulado[j] += w[i];
        }
    }
    agregados[i] = true;
}

bool respetaResistencias(){ // O(n)
    for (int j = 0; j < n; j++) {
        if(r[j] < pesoAcumulado[j]){
            return false;
        }
    }
    return true;
}

// i: posicion del producto a considerar en este nodo.
// t: suma de los pesos de los productos seleccionados hasta este nodo.
// k: cantidad de productos seleccionados hasta este nodo.
void FB(int i, int t, int k) // O(2 * n * 2^n) + O(n) = O(n * 2^n)
{
    // Caso base.
    if (i == n) { // O(1)
        if(t <= R && solucion_actual <= k && respetaResistencias()){ // O(n)
            solucion_actual = k; // O(1)
        }
        return; // O(1)
    }

    // Recursión. // O(n * 2^n)
    FB(i+1, t, k);
    agregarPeso(agregados, i); // O(n) 
    FB(i + 1, t + w[i], k + 1);
}

/* 
    mejor caso
    r[i] < p[i+1] o (p[i] > R for all i)  por factibilidad

    peor caso
    nunca se rompe ninguna resistencia
    r[i] >= suma de todos los(p[j] (V j > i)) y Peso Total <= R

*/

bool poda_factibilidad = true; // define si la poda por factibilidad esta habilitada.
bool poda_optimalidad = true; // define si la poda por optimalidad esta habilitada.

void BT(int i, int t, int k) // O(n) + O(1) + O(1) + O(n * 2^n) = O(n * 2^n)
{
    if(poda_factibilidad && (t > R || !respetaResistencias())) return; // O(n)
    if(poda_optimalidad && k+n-i < solucion_actual) return; // O(1) //! REVISAR CASO BORDE     
    // Caso base.
    if (i == n) { // O(1)
        if(t <= R && solucion_actual <= k && respetaResistencias()){ // O(n)
            solucion_actual = k; // O(1)
        }
        return; // O(1)
    }

    // Recursión. // O(n * 2^n)
    FB(i+1, t, k);
    agregarPeso(agregados, i);
    FB(i + 1, t + w[i], k + 1);

}

vector<vector<int>> M; // Memoria de PD.
const int UNDEFINED = -1;


// i: posicion del producto a considerar en este nodo.
// t: suma de los pesos de los productos seleccionados hasta este nodo.
// rp: resistencia parcial.
// acum: cantidad de productos apilados hasta este nodo.
int PD(int i, int t, int rp, int acum)
{
    if (t > R || rp < 0) return 0;
    if (i == n && t <= R) return acum;

    if(M[i][rp] == UNDEFINED){
        M[i][rp] = max(PD(i+1, t, rp, acum), PD(i+1, t+w[i], min(r[i], rp-w[i]), acum+1));
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
    pesoAcumulado.assign(n,0);
    for (int i = 0; i < n; ++i) cin >> w[i] >> r[i];

    // Ejecutamos el algoritmo y obtenemos su tiempo de ejecución.
    int optimum;
    optimum = INFTY;
    auto start = chrono::steady_clock::now();
    if (algoritmo == "FB")
    {
        FB(0,0,0);
        optimum = solucion_actual;
    }
    else if (algoritmo == "BT")
    {
        poda_optimalidad = poda_factibilidad = true;
        BT(0, 0, 0);
        optimum = solucion_actual;
    }
    else if (algoritmo == "BT-F")
    {
        poda_optimalidad = false;
        poda_factibilidad = true;
        BT(0, 0, 0);
        optimum = solucion_actual;
    }
    else if (algoritmo == "BT-O")
    {
        poda_optimalidad = true;
        poda_factibilidad = false;
        BT(0, 0, 0);
        optimum = solucion_actual;
    }
    else if (algoritmo == "DP")
    {
        // Precomputamos la solucion para los estados.
        M = vector<vector<int>>(n+1, vector<int>(R+1, UNDEFINED));
        for (int i = 0; i < n+1; ++i)
            for (int j = 0; j < R+1; ++j)
                PD(i, j, R, 0);

        // Obtenemos la solucion optima.
        optimum = PD(0, 0, R, 0);
    }
    auto end = chrono::steady_clock::now();
    double total_time = chrono::duration<double, milli>(end - start).count();

    // Imprimimos el tiempo de ejecución por stderr.
    clog << total_time << endl;

    // Imprimimos el resultado por stdout.
    cout << (optimum == INFTY ? -1 : optimum) << endl;
    return 0;
}