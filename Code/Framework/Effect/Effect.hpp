#ifndef EFFECT_HPP
#define EFFECT_HPP

#include <string>
#include <vector>
#include <Helper\Global.hpp>
#include <Framework\Effect\Technique.hpp>

namespace Framework
{
	namespace Effect
	{
		class Effect
		{
		public:
			Effect(ID3D10Device* device, const std::string& filename);
			~Effect();

			// Release the old effect file and load a new one, and enumerate all the techniques
			bool LoadEffectFile(const std::string& filename);

			// Get the number of techniques in this effect
			unsigned int GetTechniqueCount() const;

			// Get a technique by specifying its index
			Technique& GetTechniqueByIndex(unsigned int index);
			const Technique& GetTechniqueByIndex(unsigned int index) const;

		
			// Get a technique by specifying its name. If no technique by the name exists,
			// a runtime exception is raised.
			Technique& GetTechniqueByName(const std::string& name);
			const Technique& GetTechniqueByName(const std::string& name) const;

		
			// Set global or constant buffer variables
			void SetVariable(const std::string& variableName, int value);
			void SetVariable(const std::string& variableName, bool value);
			void SetVariable(const std::string& variableName, float value);
			void SetVariable(const std::string& variableName, const D3DXVECTOR4& value);
			void SetVariable(const std::string& variableName, const D3DXVECTOR3& value);
			void SetVariable(const std::string& variableName, const D3DXVECTOR2& value);
			void SetVariable(const std::string& variableName, const D3DXMATRIX& value);
			void SetVariable(const std::string& variableName, ID3D10ShaderResourceView* value);
			void SetVariable(const std::string& variableName, unsigned int elementSize, unsigned int elementCount, void* data);
			ID3D10EffectVariable* GetVariable(const std::string& variableName);
		private:
			ID3D10Device* mDevice;
			ID3D10Effect* mEffect;
	
			std::vector<Technique*> mTechniques;

		
			// Effects should not be copied. Immutable resource (at this point, since
			// no methods for modifying them are present).
			Effect(const Effect& copy);
			Effect& operator=(const Effect& copy);
		};
	}
}

#endif