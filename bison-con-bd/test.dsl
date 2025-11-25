// Archivo de prueba para FashionOpsDSL con Base de Datos

// Probar REPONER - Genera UPDATE
REPONER camisa_M 50;

// Probar APLICAR_DESCUENTO - Genera INSERT
APLICAR_DESCUENTO camisa_M 25%;

// Probar VER_STOCK - Genera SELECT
VER_STOCK camisa_M ES cantidad;

// Probar IDENTIFICAR_LENTOS - Genera SELECT con filtro
IDENTIFICAR_LENTOS DIAS MAYOR 10;

// Probar REPORTAR VENTAS
REPORTAR VENTAS DIARIO;

// Operaciones adicionales
REPONER pantalon_azul 30;
APLICAR_DESCUENTO zapato_42 15%;
IDENTIFICAR_LENTOS DIAS MAYOR 20 VENTAS MENOR 5;
