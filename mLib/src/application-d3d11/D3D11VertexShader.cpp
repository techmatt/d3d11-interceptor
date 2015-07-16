
void ml::D3D11VertexShader::load(
	GraphicsDevice &g,
	const std::string &filename,
	const std::string& entryPoint,
	const std::string& shaderModel,
	const std::vector<std::pair<std::string, std::string>>& shaderMacros)
{
	m_graphics = &g.castD3D11();
	if (!util::fileExists(filename))
	{
		std::cout << "file not found: " << filename << std::endl;
		return;
	}
	release();
	SAFE_RELEASE(m_blob);

	m_filename = filename;
	g.castD3D11().registerAsset(this);

	m_blob = D3D11Utility::CompileShader(m_filename, entryPoint, shaderModel, shaderMacros);
	MLIB_ASSERT_STR(m_blob != nullptr, "CompileShader failed");

	reset();
}

void ml::D3D11VertexShader::release()
{
	SAFE_RELEASE(m_shader);
	SAFE_RELEASE(m_standardLayout);
}

void ml::D3D11VertexShader::reset()
{
	release();

	auto &device = m_graphics->getDevice();

	D3D_VALIDATE(device.CreateVertexShader(m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_shader));

	device.CreateInputLayout(D3D11TriMesh::layout, D3D11TriMesh::layoutElementCount, m_blob->GetBufferPointer(), m_blob->GetBufferSize(), &m_standardLayout);
}

void ml::D3D11VertexShader::bind() const
{
	auto &context = m_graphics->getContext();
	context.VSSetShader(m_shader, nullptr, 0);
	context.IASetInputLayout(m_standardLayout);
}