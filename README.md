# Atividade 4
Computação Gráfica 


Gustavo Gualberto Rocha de Lemos 


 **RA**: 21008313

O aplicativo permite explorar um sistema solar, controlando livremente a câmera e se movimentando pelo espaço 3D usando o mouse e o teclado. O ambiente contém estrelas pequenas ao fundo e uma estrela parecida com o sol ao centro, um planeta orbitando à sua volta e uma nave espacial viajando. O sol emite luz, que faz com que o planeta e a nave tenham suas texturas mostradas de forma diferente dependendo da sua posição.

A câmera é criada usando a função glm::LookAT:

    m_viewMatrix = glm::lookAt(eye, eye + at, up);

Onde 'eye' é a posição da câmera, 'eye+at' é o vetor que indica a direção para onde a câmera está olhando, e up é o vetor de orientação "para cima".
Os valores iniciais desses parâmetros são:

    glm::vec3 eye{0.0f, 0.0f, 0.0f};
    glm::vec3 at{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};

Então inicialmente a câmera estará na posição 0,0,0, olhando para 0,0,-1, tendo como orientação "para cima" o vetor 0,1,0.

O módulo Window contém o funcionamento principal da aplicação:
A função onEvent lida com as entradas do usuário. Ela contém uma constante chamada "speed", que é o valor 5 multiplicado por deltatime, que é o tempo entre um quadro e outro. Esse multiplicador é aplicado a todas as movimentações da câmera para uma movimentação numa velocidade num ritmo adequado.
As teclas a, s, d, w, r, f são usadas pelo programa da seguinte maneira:

    if (event.key.keysym.sym == SDLK_a)
    eye -= glm::normalize(glm::cross(at, up)) * speed;
    if (event.key.keysym.sym == SDLK_d)
    eye += glm::normalize(glm::cross(at, up)) * speed;

As teclas 'a' e 'd' fazem a posição da câmera mudar lateralmente. Isso acontece ao incrementar ou decrementar o valor de 'eye', que é a posição da câmera, com um vetor normal à multiplicação vetorial entre at e up. Isso funciona porque a multiplicação vetorial entre dois vetores resulta em um vetor perpendicular aos dois, que nesse caso entre at e up, será um vetor lateral à posição da câmera.

As teclas 'w' e 's' avançam ou recuam a câmera na direção em que está olhando:

    if (event.key.keysym.sym == SDLK_w)
        eye += speed * at;
    if (event.key.keysym.sym == SDLK_s)
        eye -= speed * at;

Isso é feito incrementando ou decrementando o valor 'eye' com o vetor 'at', sempre multiplicado por 'speed'.

Por fim, as teclas 'r' e 'f' mudam a posição verticalmente:

    if (event.key.keysym.sym == SDLK_r)
        eye += speed * up;
    if (event.key.keysym.sym == SDLK_f)
        eye -= speed * up;

Com elas a posição é somada ou subtraída com o vetor up.

O mouse wheel controla o valor do ângulo de campo de visão, field of view:

    if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y > 0 && m_FOV > 15.0f)
        m_FOV -= 1.0f;
        if (event.wheel.y < 0 && m_FOV < 70.0f)
        m_FOV += 1.0f;
    }

O ângulo é usado para dar um efeito de zoom, e é limitado para ficar entre os valores de 15 até 70.

A posição do mouse é usada para calcular a direção para onde olhar:

    if (event.type == SDL_MOUSEMOTION) {
        SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    }

Essa função guarda as coordenadas do mouse na variável mousePosition.

A função onCreate define os parâmetros iniciais da aplicação, como a localização dos shaders e modelos na pasta Assets, a câmera e as posições e eixo de rotação dos objetos em cena. São usados dois programas: m_program, com os shaders de textura e m_program2, que usa shaders apenas com cores. m_program será usado para renderizar os objetos com texturas, e m_program2 será para as estrelas de fundo, que usam apenas a cor branca no modelo.

Para a criação dos modelos é usado o módulo Model, com cada modelo sendo um objeto dele. 
Por exemplo, para a nave espacial:

    ufo.loadDiffuseTexture(assetsPath + "13884_Diffuse.jpg");
    ufo.loadObj(assetsPath + "13884_UFO_Saucer_v1_l2.obj");
    ufo.setupVAO(m_program);

O módulo Model carrega o arquivo de imagem que contém as texturas com a função loadDiffuseTexture, a função loadObj para carregar o modelo e também cria o vertex array object para ele dentro de m_program.

Os coeficientes de reflexo são definidos de acordo com o modelo:

    m_Ka = ufo.getKa();
    m_Kd = ufo.getKd();
    m_Ks = ufo.getKs();
    m_shininess = ufo.getShininess();

m_Ka é o componente de ambiente, m_Kd é o componente de luz difusa, m_Ks é o componente especular e m_shininess representa o brilho. Como os modelos não têm esses valores definidos individualmente, o módulo Model aplica valores padrões a esses componentes:

    m_Ka = {0.1f, 0.1f, 0.1f, 1.0f};
    m_Kd = {0.7f, 0.7f, 0.7f, 1.0f};
    m_Ks = {1.0f, 1.0f, 1.0f, 1.0f};
    m_shininess = 25.0f;

Esses são os valores aplicados a todos os objetos, que determinam como eles reagem quando a luz incide sobre eles.

m_viewMatrix define a posição inicial da câmera, por onde o usuário verá toda a cena:

    m_viewMatrix = glm::lookAt(eye, eye + at, up);

O seguinte loop define a posição e rotação de cada estrela:

    for (auto &star : m_stars) {
        randomizeStar(star);
    }

m_stars é um array com 1000 structs de nome Star, que têm uma posição e um eixo de rotação cada uma. A função randomizeStar define uma posição e um eixo de rotação para cada uma:

    std::uniform_real_distribution<float> distPosXY(-50.0f, 50.0f);
    std::uniform_real_distribution<float> distPosZ(-150.0f, -50.0f);
    star.m_position =
        glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                    distPosZ(m_randomEngine));

    // Random rotation axis
    star.m_rotationAxis = glm::sphericalRand(1.0f);

a posição delas é definida aleatoriamente, mas com os eixos x e y variando entre -50 e 50, e o eixo z variando entre -150 e -50. Esses valores no eixo z garantem que as estrelas estarão distantes da câmera no início da aplicação. O eixo de rotação das estrelas é definido aleatoriamente.

Por fim, a posição e eixo de rotação dos demais objetos é definida, cada um deles também sendo uma Struct com um valor de posição e outro de eixo de rotação:

    m_planet.m_position = glm::vec3(20.0f, 0.0f, -100.0f);
    m_planet.m_rotationAxis = glm::vec3(0.0f, 1.0f, 1.0f);

    m_sun.m_position = glm::vec3(0.0f, 0.0f, -90.0f);
    m_sun.m_rotationAxis = glm::vec3(0.0f, 1.0f, 1.0f);

    m_ufo.m_position = glm::vec3(10.0f, 1.0f, -30.0f);
    m_ufo.m_rotationAxis = glm::vec3(0.0f, 0.0f, 1.0f);

A posição inicial desses objetos é fixa, e foi pensada para criar uma boa primeira visão para o usuário. O sol estará no meio da visão inicial, com o planeta e a nave do lado direito.

A função onUpdate lida com as mudanças a cada quadro:

    m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);
    m_angle2 = glm::wrapAngle(m_angle2 + glm::radians(2.0f) * deltaTime);

m_angle vai mudar a cada segundo, fazendo com que os modelos das estrelas e da nave girem 90 graus por segundo. m_angle2 por sua vez determina a velocidade do giro do planeta na órbita do sol e em torno de si mesmo, sendo definido como 2 graus por segundo.

Para fazer a movimentação do mouse mudar a direção para onde a câmera olha é necessário que se façam as seguintes operações trigonométricas:

    float xoffset = mousePosition.x - lastX;
    float yoffset = lastY - mousePosition.y;
    lastX = mousePosition.x;
    lastY = mousePosition.y;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    at = glm::normalize(direction);

    m_viewMatrix = glm::lookAt(eye, eye + at, up);

xoffset e yoffset guardam a diferença da posição do mouse entre um quadro e outro, e são multiplicados pelo valor de sensitivity, que define a velocidade que a câmera vai se movimentar. O valor pitch é o eixo de rotação vertical, e yaw é o eixo horizontal. A direção da câmera é definida pelas operações trigonométricas entre elas: O valor em x é o cosseno do eixo horizontal multiplicado pelo cosseno do eixo vertical, o valor em y é o seno do eixo vertical, e o valor z é o seno do eixo horizontal multiplicado pelo cosseno do eixo vertical.

As mudanças de posições dos objetos também são definidas na função onUpdate:
A nave irá se mover principalmente para a esquerda, porque a sua posição tem os valores decrescidos de acordo com esse vetor:

    m_ufo.m_position -= deltaTime * glm::vec3{0.3f, 0.03f, -0.2f};

A posição do planeta é definida de acordo com as seguintes operações trigonométricas:

    m_planet.m_position.x = m_sun.m_position.x +
                            (50.0f - m_sun.m_position.x) * cos(m_angle2) -
                            (-80.0f - m_sun.m_position.z) * sin(m_angle2);

    m_planet.m_position.z = m_sun.m_position.z +
                            (50.0f - m_sun.m_position.x) * sin(m_angle2) +
                            (-80.0f - m_sun.m_position.z) * cos(m_angle2);

Essas operações seguem a fórmula de rotação de objeto ao redor de um ponto, nesse caso é o planeta em volta do sol. Os eixos X e Z são usados para fazer com que a órbita seja no plano do eixo X, com os valores de Y inalterados.

Também é chamada a função CheckCollision, que evita que a câmera encoste nos objetos. Para isso ela calcula a distância entre a câmera e o objeto, e caso a distância seja menor que 1 + a escala do objeto, ela "empurra" a câmera para trás. Por exemplo, o planeta:

    auto const distancePlanet{glm::distance(eye, m_planet.m_position)};
    if (distancePlanet < 15.0f) {
        eye -= 5.0f * deltaTime * at;
    }

Se a distância entre o planeta e a câmera for menor que 15, a câmera se move para trás, na mesma direção do vetor at.

A função onPaint faz a renderização dos objetos, usando matrizes e os shaders. A direção da luz é definida assim:

    auto const lightDirRotated{glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)};

Esse vetor determina que a luz virá exatamente da posição onde está o sol, na direção positiva do eixo Z, o que faz com que o planeta e a nave tenham suas aparências modificadas dependendo das suas posições em relação a esse vetor. Isso foi feito para simular a iluminação que o sol de um sistema solar gera nos objetos que estão na sua proximidade.

A renderização dos objetos é determinada por transformações nas matrizes. Por exemplo, a nave espacial:

    modelMatrixUfo = glm::translate(modelMatrixUfo, m_ufo.m_position);
    modelMatrixUfo = glm::scale(modelMatrixUfo, glm::vec3(0.8f));
    modelMatrixUfo =
        glm::rotate(modelMatrixUfo, 80.0f, glm::vec3{1.0f, 0.0f, 0.0f});
    modelMatrixUfo = glm::rotate(modelMatrixUfo, m_angle, m_ufo.m_rotationAxis);

    // Set uniform variable
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrixUfo[0][0]);

    ufo.render();

a matriz do modelo passa pela função glm::translate, que indica a posição de acordo com o valor em m_position, a função glm::scale indica o tamanho do modelo e por fim o modelo é girado de acordo com os valores da variável m_angle.

A UI tem um quadro no canto da tela, com um botão para voltar a câmera a sua posição inicial, e um botão para mostrar os controles:

    if (ImGui::Button("Voltar ao Início", ImVec2(0, 0))) {
            eye = glm::vec3{0.0f, 0.0f, 0.0f};
        }
        if (ImGui::Button("Mostrar Controles", ImVec2(0, 0))) {
            control = !control;
        }

Se o botão "Voltar ao início" for apertado, a câmera volta à posição 0,0,0, e se o botão "Mostrar Controles" for pressionado, a variável booleana control, que é iniciada com valor false, tem seu valor invertido, ficando true. Isso faz a janela que mostra os controles aparecer:

    if (control) {
        ImGuiWindowFlags const flags{ImGuiWindowFlags_NoNavInputs |
                                    ImGuiWindowFlags_NoDecoration};

        ImGui::SetNextWindowPos(
            ImVec2(m_viewportSize.x / 5.0f, (m_viewportSize.y) / 80.0f));
        ImGui::SetNextWindowSize(ImVec2(250, 170));
        ImGui::Begin("Widget window", nullptr, flags);
        ImGui::Text("tecla a = Mover para a esquerda");
        ImGui::Text("tecla d = Mover para a direita");
        ImGui::Text("tecla w = Mover para frente");
        ImGui::Text("tecla s = Mover para trás");
        ImGui::Text("tecla r = Mover para cima");
        ImGui::Text("tecla f = Mover para baixo");
        ImGui::Text("scroll wheel = Zoom");

        if (ImGui::Button("Ocultar Controles", ImVec2(0, 0))) {
        control = !control;
        }
        ImGui::End();
    }

Ela mostra a janela com todos os comandos, e um botão "Ocultar Controles", que inverte o valor de control novamente, fazendo a janela original voltar a aparecer.    