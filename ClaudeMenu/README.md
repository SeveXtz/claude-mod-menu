# Claude Code — mod menu

Menu externo em Dear ImGui + DirectX 11, tema escuro com as cores do Claude e o mascote em pixel art.

## Compilar (Visual Studio 2022)

1. Instale o workload **"Desenvolvimento para desktop com C++"** (já vem com CMake).
2. No Visual Studio: **Arquivo → Abrir → Pasta...** e escolha a pasta `ClaudeMenu`.
3. Ele lê o `CMakeLists.txt` e baixa o ImGui sozinho (precisa de internet na primeira vez).
4. Escolha a configuração (**x64-Release**, ou **x86** se o jogo for 32-bit) e rode `ClaudeMenu.exe`.

## Adicionar seus ponteiros

Abra `src/Menu.h` e procure o bloco **EDITE AQUI**. Cada opção tem um campo `ptr`:

```cpp
{ "Vida infinita", "Congela a vida...", false, 100.f, 1.f, 9999.f,
  { L"game.exe", 0x0123ABC0, { 0x10, 0x48, 0x2C } } },
```

- `false` = escreve `int`, `true` = escreve `float`
- `100.f, 1.f, 9999.f` = valor inicial, mínimo e máximo do slider
- `ptr` = módulo + offset base + offsets da cadeia (o mesmo formato do Cheat Engine)

Com a opção ligada, o valor é escrito a cada frame, então ele fica congelado.

## Uso

- Aba **Config**: digite o nome do processo (ex.: `game.exe`) e clique em **Conectar**
- **INSERT**: mostra/oculta o menu (os valores continuam sendo aplicados)
- Arraste pelo topo para mover a janela
