#version 330


out vec4 pixelColor; //Zmienna wyjsciowa fragment shadera. Zapisuje sie do niej ostateczny (prawie) kolor piksela
in vec4 iC;

in vec4 l;
in vec4 n;
in vec4 v;
uniform sampler2D textureMap0;
in vec2 iTexCoord0;

void main(void) {


	vec4 ml = normalize(l);
	vec4 mn = normalize(n);
	vec4 mv = normalize(v);
	vec4 mr=reflect(-ml,mn); //Wektor odbity

	float nl = clamp(dot(mn, ml), 0, 1); //Kosinus kąta pomiędzy wektorami n i l.
	float rv = pow(clamp(dot(mr, mv), 0, 1), 25); // Kosinus kąta pomiędzy wektorami r i v podniesiony do 25 potęgi

	vec4 kd=texture(textureMap0,iTexCoord0);

	pixelColor= vec4(nl * kd.rgb, kd.a) + vec4(rv, rv, rv, 0); //Wyliczenie modelu oświetlenia (bez ambient);
}
