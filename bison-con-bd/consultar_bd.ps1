# Script PowerShell para consultar la base de datos fashion_ops.db
# Ejecutar: .\consultar_bd.ps1

$db = "fashion_ops.db"

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "  CONSULTAS BASE DE DATOS FASHION OPS" -ForegroundColor Cyan
Write-Host "========================================`n" -ForegroundColor Cyan

# Verificar si existe la BD
if (-not (Test-Path $db)) {
    Write-Host "ERROR: No se encuentra la base de datos $db" -ForegroundColor Red
    exit 1
}

# 1. PRODUCTOS
Write-Host "===== TABLA PRODUCTOS =====" -ForegroundColor Yellow
sqlite3 $db -header -column "SELECT * FROM productos;"
Write-Host ""

# 2. DESCUENTOS (últimos 10)
Write-Host "===== DESCUENTOS (Últimos 10) =====" -ForegroundColor Yellow
sqlite3 $db -header -column "SELECT * FROM descuentos ORDER BY id DESC LIMIT 10;"
Write-Host ""

# 3. VENTAS
Write-Host "===== TABLA VENTAS =====" -ForegroundColor Yellow
$ventas = sqlite3 $db "SELECT COUNT(*) FROM ventas;"
if ($ventas -eq "0") {
    Write-Host "No hay ventas registradas" -ForegroundColor Gray
} else {
    sqlite3 $db -header -column "SELECT * FROM ventas LIMIT 10;"
}
Write-Host ""

# 4. RESUMEN DE DESCUENTOS POR PRODUCTO
Write-Host "===== RESUMEN DESCUENTOS POR PRODUCTO =====" -ForegroundColor Yellow
sqlite3 $db -header -column @"
SELECT 
    sku, 
    COUNT(*) as total_descuentos, 
    ROUND(AVG(porcentaje), 2) as promedio_descuento,
    ROUND(MIN(porcentaje), 2) as min_descuento,
    ROUND(MAX(porcentaje), 2) as max_descuento
FROM descuentos 
GROUP BY sku;
"@
Write-Host ""

# 5. ESTADÍSTICAS GENERALES
Write-Host "===== ESTADÍSTICAS GENERALES =====" -ForegroundColor Yellow
Write-Host "Total productos: " -NoNewline
sqlite3 $db "SELECT COUNT(*) FROM productos;"
Write-Host "Total descuentos aplicados: " -NoNewline
sqlite3 $db "SELECT COUNT(*) FROM descuentos;"
Write-Host "Total ventas: " -NoNewline
sqlite3 $db "SELECT COUNT(*) FROM ventas;"
Write-Host "Stock total en inventario: " -NoNewline
sqlite3 $db "SELECT SUM(stock) FROM productos;"
Write-Host ""

Write-Host "========================================`n" -ForegroundColor Cyan
