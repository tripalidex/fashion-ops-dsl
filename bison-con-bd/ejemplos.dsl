// ================================================
// EJEMPLOS COMPLETOS - FashionOpsDSL
// ================================================

// ----------------
// 1. GESTIÓN DE INVENTARIO
// ----------------

// Reponer stock de productos
REPONER camisa_M 50;
REPONER pantalon_azul 30;
REPONER zapato_42 20;

// Consultar stock actual
VER_STOCK camisa_M ES stock_camisas;
VER_STOCK pantalon_azul ES stock_pantalones;

// ----------------
// 2. APLICACIÓN DE DESCUENTOS
// ----------------

// Descuentos fijos
APLICAR_DESCUENTO camisa_M 25%;
APLICAR_DESCUENTO pantalon_azul 15%;
APLICAR_DESCUENTO zapato_42 30%;

// ----------------
// 3. IDENTIFICACIÓN DE PRODUCTOS LENTOS
// ----------------

// Productos sin ventas por más de 10 días
IDENTIFICAR_LENTOS DIAS MAYOR 10;

// Productos sin ventas por más de 20 días
IDENTIFICAR_LENTOS DIAS MAYOR 20;

// Productos lentos con ventas bajas
IDENTIFICAR_LENTOS DIAS MAYOR 15 VENTAS MENOR 5;

// ----------------
// 4. REPORTES DE VENTAS
// ----------------

// Reporte diario
REPORTAR VENTAS DIARIO;

// Reporte mensual
REPORTAR VENTAS MENSUAL;

// ----------------
// 5. VARIABLES Y CONSTANTES
// ----------------

// Declarar variables
VARIABLE umbral_dias = 30;
VARIABLE descuento_agresivo = 40;
CONSTANTE precio_minimo = 10;

// ----------------
// 6. ESTRUCTURAS DE CONTROL
// ----------------

// Condicional simple
SI stock_camisas MENOR 20 ENTONCES
    REPONER camisa_M 50;
FIN

// Condicional con alternativa
SI stock_pantalones MAYOR 100 ENTONCES
    APLICAR_DESCUENTO pantalon_azul 20%;
SINO
    REPONER pantalon_azul 30;
FIN

// ----------------
// 7. ITERACIÓN CON ARREGLOS
// ----------------

// Declarar arreglo de SKUs
ARREGLO productos_descuento = ["camisa_M", "pantalon_azul", "zapato_42"];

// Iterar sobre productos
PARA producto EN productos_descuento HAZ
    APLICAR_DESCUENTO producto 15%;
FIN

// ----------------
// 8. OPERACIONES COMBINADAS
// ----------------

// Calcular nuevo precio con descuento
VARIABLE precio_original = 100;
VARIABLE descuento = 25;
VARIABLE precio_final = precio_original - (precio_original * descuento / 100);

// Reponer basado en ventas proyectadas
VARIABLE ventas_esperadas = 50;
VARIABLE stock_seguridad = 20;
VARIABLE cantidad_reponer = ventas_esperadas + stock_seguridad;

REPONER camisa_M cantidad_reponer;

// ----------------
// 9. CASOS DE USO REALES
// ----------------

// Liquidación de temporada
IDENTIFICAR_LENTOS DIAS MAYOR 45;
APLICAR_DESCUENTO camisa_M 50%;
APLICAR_DESCUENTO pantalon_azul 40%;

// Reabastecimiento inteligente
VER_STOCK camisa_M ES stock_actual;
SI stock_actual MENOR 30 ENTONCES
    REPONER camisa_M 100;
FIN

// Promoción relámpago
APLICAR_DESCUENTO zapato_42 35%;
REPORTAR VENTAS DIARIO;

// ----------------
// 10. ANÁLISIS DE INVENTARIO
// ----------------

// Productos críticos (muy lento + stock alto)
IDENTIFICAR_LENTOS DIAS MAYOR 60 VENTAS MENOR 3;

// Aplicar descuentos agresivos
APLICAR_DESCUENTO camisa_M 45%;
APLICAR_DESCUENTO pantalon_azul 40%;

// Verificar impacto
REPORTAR VENTAS MENSUAL;
