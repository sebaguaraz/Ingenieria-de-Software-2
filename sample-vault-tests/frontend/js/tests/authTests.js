/**
 * Test: POST /api/auth/login
 */
testUtils.createTestButton("Test Login Correcto (Pepe y 12345)", async (btn) => {
    const response = await fetch('/api/auth/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username: 'pepe', password: '12345' }) // Usamos pepe hardcodeado
    });

    const data = await response.json();
    
    if (response.ok) {
        testUtils.log(data);
        testUtils.setSuccess(btn);
    }
});

testUtils.createTestButton("Test Login - Password Incorrecto (Pepe y 123)", async (btn) => {
    const response = await fetch('/api/auth/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username: 'pepe', password: '123' }) // Usamos pepe hardcodeado
    });

    const data = await response.json();
    console.log("Response status:", response.status);
    console.log("Response ok:", response.ok);
    if (response.status === 401) {
        testUtils.log(data)
        testUtils.setSuccess(btn);
    }
});

testUtils.createTestButton("Test Login - Usuario Incorrecto (Juan y 12345)", async (btn) => {
    const response = await fetch('/api/auth/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ username: 'juan', password: '12345' }) // Usamos pepe hardcodeado
    });

    const data = await response.json();

    if (response.status === 401) {
        testUtils.log(data)
        testUtils.setSuccess(btn);
    }
});


testUtils.createTestButton("Test Registro - Usuario Nuevo", async function (button) {

    const user = {
        username: `seba${Date.now()}`,
        password: "seba123"
    }

    const objectConfig = {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(user)
    }

    const response = await fetch("/api/auth/register", objectConfig)
    const result = await response.json()

    if (response.status === 201) {
        testUtils.log(result)
        testUtils.setSuccess(button)
    }


})

