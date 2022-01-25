﻿#pragma once
#define PY_SSIZE_T_CLEAN
#include "../include/Python.h"
#include <Global.h>

#define Py_PARSE(format,...) if (!PyArg_ParseTuple(args, format ":" __FUNCTION__, __VA_ARGS__))return nullptr
#define Py_KERWORDS_LIST(...) static const char* kwlist[]{ __VA_ARGS__,nullptr }
#define Py_PARSE_WITH_KERWORDS(format,...) if (!PyArg_ParseTupleAndKeywords(args, kwds, format ":" __FUNCTION__, const_cast<char**>(kwlist), __VA_ARGS__))return nullptr

#define Py_RETURN_ERROR(str) return PyErr_SetString(PyExc_Exception, str), nullptr
#define Py_RETURN_ERROR_FORMAT(str,...) return PyErr_Format(PyExc_Exception, str, __VA_ARGS__), nullptr
#define Py_PRINT_REFCOUNT(obj) logger.info(#obj " 的引用计数 : {}",  obj->ob_refcnt)

//#define Py_BEGIN_CALL\
//	int _has_gil = PyGILState_Check();\
//	PyGILState_STATE _gil_state = PyGILState_LOCKED;\
//	if (!_has_gil)_gil_state = PyGILState_Ensure();\
//	Py_BEGIN_ALLOW_THREADS;\
//	Py_BLOCK_THREADS
//#define Py_END_CALL\
//	Py_UNBLOCK_THREADS\
//	Py_END_ALLOW_THREADS;\
//	if (!_has_gil)PyGILState_Release(_gil_state)
class PyGILGuard {
public:
	PyGILGuard() {
		gil_ = PyGILState_Ensure();
	}
	~PyGILGuard() {
		PyGILState_Release(gil_);
	}

private:
	PyGILState_STATE gil_;
};

//字符串转Unicode
inline PyObject* ToPyStr(std::string_view str) {
	return PyUnicode_InternFromString(str.data());
	//return PyUnicode_FromStringAndSize(str.data(), str.length());
}
//list转vector
inline std::vector<std::string> ToStrArray(PyObject* list) {
	std::vector<std::string> arr;
	if (PyList_Check(list)) {
		for (Py_ssize_t i = 0; i < PyList_Size(list); i++) {
			arr.push_back(PyUnicode_AsUTF8(PyList_GetItem(list, i)));
		}
	}
	return arr;
}
//Vec3转list
inline PyObject* ToList(Vec3 vec) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyFloat_FromDouble(vec.x));
	PyList_SetItem(list, 1, PyFloat_FromDouble(vec.y));
	PyList_SetItem(list, 2, PyFloat_FromDouble(vec.z));
	return list;
}
//Vec3转list
inline PyObject* ToList(Vec3* vec) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyFloat_FromDouble(vec->x));
	PyList_SetItem(list, 1, PyFloat_FromDouble(vec->y));
	PyList_SetItem(list, 2, PyFloat_FromDouble(vec->z));
	return list;
}
//方块坐标转list
inline PyObject* ToList(BlockPos* bp) {
	PyObject* list = PyList_New(3);
	PyList_SetItem(list, 0, PyLong_FromLong(bp->x));
	PyList_SetItem(list, 1, PyLong_FromLong(bp->y));
	PyList_SetItem(list, 2, PyLong_FromLong(bp->z));
	return list;
}
inline void TupleToString(string& s, PyObject* t) {
	for (size_t i = 0; i < PyTuple_Size(t); i++) {
		s += PyUnicode_AsUTF8(PyObject_Repr(PyTuple_GetItem(t, i)));
		s += '\n';
	}
}
//打印错误信息
inline void PrintPythonError() {
	if (PyErr_Occurred()) {
		PyObject* type, * value, * traceback;
		PyErr_Fetch(&type, &value, &traceback);

		PyObject* info = reinterpret_cast<PyBaseExceptionObject*>(value)->args;

		string errs;

		errs += PyUnicode_AsUTF8(PyTuple_GetItem(info, 0));
		errs += '\n';
		TupleToString(errs, PyTuple_GetItem(info, 1));

		logger.error("{}", errs);

		PyErr_Restore(type, value, traceback);
	}
}