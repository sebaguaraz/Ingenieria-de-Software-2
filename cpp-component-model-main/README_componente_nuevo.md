# Componente: Validador de Texto Vacío

## 1. Objetivo general

- Componente llamado `TextValidatorComponent`.
- Su función es recibir un texto y determinar si está vacío o no.
- El resultado del componente debe permitir al Host saber si el texto es válido para continuar con el procesamiento.

## 2. Justificación como componente independiente

- Se considera un componente porque cumple una tarea concreta.
- Puede reutilizarse en distintos programas.
- Expone un contrato claro entre el HostApp y el componente.

## 3. Alcance funcional

- El componente deberá analizar una cadena de texto de entrada.
- El componente deberá identificar si el texto está vacío.
- El componente deberá devolver un resultado simple.
- El componente deberá informar errores mediante códigos de retorno.

## 4. Entrada y salida

- Entrada:
  - un texto en formato `const char*`.

- Salida:
  - un resultado que indique si el texto está vacío o no.

## 5. Interfaz esperada

- El componente deberá heredar de `IComponent`.
- Deberá existir una interfaz específica, por ejemplo `ITextValidator`.
- Ejemplo de operación pública:
  - `virtual validate_text(const char* input) = 0`

## 6. Requisitos del contrato ABI

- El componente deberá exponer funciones con `extern "C"` para crear la instancia y para destruirla, usadas en ModuleManager.
- Las funciones mínimas deberán ser:
  - `get_api_version()`
  - `create_component()`
  - `destroy_component()`

- `get_api_version()` deberá devolver la versión del contrato.
- `create_component()` deberá crear una instancia válida.
- `destroy_component()` deberá liberar correctamente la instancia creada.

## 7. Requisitos de seguridad y estabilidad

- Toda operación interna deberá estar protegida con `try-catch (...)`.
- Si ocurre un error interno, el componente deberá devolver un código controlado.
- El componente no deberá exponer objetos complejos como `std::string` por el ABI.

## 8. Conclusión

- `TextValidatorComponent` es un ejemplo de componente porque se implementa a partir de su interfaz `ITextValidator` (realiza una tarea), su instanciación es manejada por el metodo `std::shared_ptr<InterfaceType> create_instance(const std::string& module_name)` de la clase moduleManager, que carga el módulo dinámico y crea la instancia correspondiente.
- El `class Application`, es quien hace uso de la instancia para acceder al metodo `validate_text(const char* input)` y devolver la respuesta al main.cpp
- Cumple con el modelo de componentes porque oculta su implementacion, solo expone que parametros recibe, que retorna y que metodos puede acceder.
- No depende de la logica de HostApp (Application y ModuleManager) ni de otro modulo para funcionar.
- No depende de la lógica del HostApp (Aplication y ModuleManager) ni de otro módulo para funcionar.