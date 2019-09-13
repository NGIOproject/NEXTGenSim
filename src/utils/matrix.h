/*
* Copyright 2007 Francesc Guim Bernat & Barcelona Supercomputing Centre (fguim@pcmas.ac.upc.edu)
* Copyright 2019 Daniel Rivas & Barcelona Supercomputing Centre (daniel.rivas@bsc.es)
* Copyright 2015-2019 NEXTGenIO Project [EC H2020 Project ID: 671951] (www.nextgenio.eu)
*
* This file is part of NEXTGenSim.
*
* NEXTGenSim is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEXTGenSim is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with NEXTGenSim.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef UTILSMATRIX_H
#define UTILSMATRIX_H

namespace Utils {

/**
@author Francesc Guim,C6-E201,93 401 16 50, -- Roger Ferrer -- BSC 
*/

template <class T>
struct Matrix
{
    public :
        struct Row
        {
            private:
                // La fila
                T* _fila;
                // Aix�pot servir per comprovacio d'errors
                int _columnes;

                // El constructor d'una fila, nom� Matrix
                // pot crear Row ...
                Row(T* t, int columnes)
                    : _fila(t), _columnes(columnes)
                {
                }

                // ... gr�ies a aquest friend
                friend class Matrix;

            public:
                // L'operador []
                T& operator [](int columna)
                {
                    return _fila[columna];
                }
        };

        // Constructor d'una Matrix
        Matrix(int files, int columnes)
            : _files(files), _columnes(columnes)
        {
            // Creem les files
            _matriu = new T* [_files];
            // I les columnes
            for (int _i = 0; _i < _files; _i++)
            {
                _matriu[_i] = new T[_columnes];
            }
        }


        // Destructor
        ~Matrix()
        {
            // Destruim les columnes
            for (int _i = 0; _i < _files; _i++)
            {
                delete[] _matriu[_i];
            }
            // Creem les files
            delete[] _matriu;
        }

        // Aixo retorna tota una Row 
        Row operator[](int fila)
        {
            return Row(_matriu[fila], _columnes);
        }
    private:
        T** _matriu;
        int _files;
        int _columnes;
};


}

#endif
