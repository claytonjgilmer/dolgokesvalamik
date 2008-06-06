#ifndef _PHYSICSBODY_H_
#define _PHYSICSBODY_H_

namespace physics
{
	class Body
	{
	public:
		Body(const BodyDesc* i_Desc);
		~Body();

		Shape* CreateShape(const ShapeDesc* i_ShapeDesc);
		void ReleaseShape(Shape* i_Shape);

		void AddForce(const Vector3& i_Force);
		void AddForce(const Vector3& i_Force, const Vector3& i_Position);
		void AddTorque(const Vector3& i_Torque);
		void SetForce(const Vector3& i_Force);
		void SetTorque(const Vector3& i_Torque);

		void GetPosition(Matrix4x3& o_Position) const;
		const Matrix4x3& GetPosition() const;
		void SetPosition(const Matrix4x3& i_Position);

		Matrix4x3 GetExtrapolatedPosition(float i_DeltaTime) const;

		void GetTranslation(Vector3& o_Translation) const;
		const Vector3& GetTranslation() const;
		void SetTranslation(const Vector3& i_Translation);

		void Translate(const Vector3& i_Offset);

		void GetOrientation(Matrix3x3& o_Orientation) const;
		void SetOrientation(const Matrix3x3& i_Orientation);

		float	GetMass() const;
		void	SetMass(float i_Mass);

		Vector3	GetInertia() const;
		void	SetInertia(const Vector3& i_Inertia);

		Vector3	GetVelocity() const;
		void	SetVelocity(const Vector3& i_Velocity);

		Vector3	GetAngularVelocity() const;
		void	SetAngularVelocity(const Vector3& i_AngVel);

		void	SetVelocityDamping(float i_VelocityDamping);
		float	GetVelocityDamping();

		void	SetAngularVelocityDamping(float i_AngularVelocityDamping);
		float	GetAngularVelocityDamping();

		bool	IsActive() const;
		bool	IsSleeping() const;
		bool	IsStatic() const;

		Scene*	GetScene() const;

		void	GetBounding(Vector3& o_BoundingMin, Vector3& o_BoundingMax) const;

		void SetCollisionCallback(CollisionCallback* i_CollisionCallback);
		void ClearCollisionCallback();

		void* GetUserData() const;

		int GetShapeNum() const;
		Shape* GetShape(int i_Index) const;



		ContactInformation* GetContactInfo(Body* i_Other) const;
		void AddContactInfo(ContactInformation* i_Info);
		void EraseContactInfo(const ContactInformation* i_Info);
		void EraseAllContactInfo();



		void AddJoint(Joint* i_Joint);
		void RemoveJoint(Joint* i_Joint);
		void RemoveAllJoints();

		bool IsMoving() const;
		bool NotMoving() const;
		bool IsInactive() const;
		bool IsDynamic() const;
		void SetToActive();
		void SetToInactive();
		void SetToSleeping();
		void ClearSleeping();
		bool IsCollider() const;
		void SetCollider();
		void UnsetCollider();
		void SetDead();
		bool IsDead() const;

		void RecalcBounding();
		void ReleaseAllShapes();


		Scene*		m_Scene;

		Matrix4x3			m_Position;
		Vector3			m_Velocity;
		Vector3			m_AngularVelocity;

		float			m_MaxVelocity;
		float			m_MaxAngularVelocity;

		Vector3			m_VelocityPositionCorrection;
		Vector3			m_AngularVelocityPositionCorrection;
		Vector3			m_Force;
		Vector3			m_Torque;

		float			m_InvMass;
		Vector3			m_InvInertia;
		Matrix3x3		m_InvInertiaAbs;

		AABB				m_Bounding;

		Matrix4x3			m_PreviousPosition;

		Matrix4x3			m_LastFrame;

		int				m_BodyFlags;

		float			m_VelocityDamping;
		float			m_AngularVelocityDamping;

		int				m_IdleStepsLeft;

		int				m_SolverStamp;
		int				m_SolverIndex;

		BroadPhaseObject* m_BroadPhaseObject;

		unsigned short m_Index;
		unsigned short	m_GroupIndex;

		CollisionCallback* m_CollisionCallback;
		void*				m_UserData;

		typedef Dyn::vector<Shape*> tShapeArray;
		tShapeArray	m_Shapes;

		typedef Dyn::vector<ContactInformation*> tContactInfoArray;
		tContactInfoArray m_ContactInfo;

		Dyn::vector<Joint*>	m_JointList;
	};

}


#endif