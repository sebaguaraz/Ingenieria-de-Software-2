/**
 * Test: GET /api/admin/users (Requiere Login Admin)
 */


testUtils.createTestButton("Test Admin: Listar Usuarios con Login Admin Correcto", async (btn) => {
    // Primero hacemos un login rápido como admin para obtener el token adecuado
    const loginRes = await fetch('/api/auth/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username: 'admin', password: '12345' })
    });

    const { token } = await loginRes.json();

    const response = await fetch('/api/admin/users', {
        headers: { 'Authorization': `Bearer ${token}` }
    });

    const data = await response.json();
    testUtils.log(data);
    if (response.ok) testUtils.setSuccess(btn);
});

testUtils.createTestButton("Test Seguridad - Productor accediendo a Admin", async function (button) {

    await okLogin(); // Login como productor para obtener un token válido
    const token = localStorage.getItem('test_token');


    const response = await fetch("/api/admin/users", {
        headers: { "Authorization": `Bearer ${token}` }
    })

    const result = await response.json()

    if(response.status === 403) {
        testUtils.log(result)
        testUtils.setSuccess(button)
    }

})