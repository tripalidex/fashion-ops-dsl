-- Script SQL para visualizar la estructura y datos de la base de datos
-- Ejecutar con: sqlite3 fashion_ops.db < consultas.sql

.mode column
.headers on
.width 15 25 10 10 15

-- Mostrar estructura de tablas
.echo ========================================
.echo ESTRUCTURA DE TABLAS
.echo ========================================

.schema productos
.echo
.schema descuentos
.echo
.schema ventas
.echo

-- Consultar productos
.echo ========================================
.echo PRODUCTOS EN INVENTARIO
.echo ========================================
SELECT * FROM productos;

.echo
.echo ========================================
.echo DESCUENTOS APLICADOS
.echo ========================================
SELECT d.*, p.nombre 
FROM descuentos d 
LEFT JOIN productos p ON d.sku = p.sku
ORDER BY d.fecha_aplicacion DESC;

.echo
.echo ========================================
.echo PRODUCTOS CON MOVIMIENTO LENTO
.echo ========================================
SELECT sku, nombre, stock, dias_sin_venta 
FROM productos 
WHERE dias_sin_venta > 10
ORDER BY dias_sin_venta DESC;

.echo
.echo ========================================
.echo HISTORIAL DE VENTAS
.echo ========================================
SELECT v.*, p.nombre 
FROM ventas v 
LEFT JOIN productos p ON v.sku = p.sku
ORDER BY v.fecha DESC;

.echo
.echo ========================================
.echo RESUMEN DE STOCK
.echo ========================================
SELECT 
    COUNT(*) as total_productos,
    SUM(stock) as total_items,
    AVG(precio) as precio_promedio,
    SUM(stock * precio) as valor_inventario
FROM productos;
