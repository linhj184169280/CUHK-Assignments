using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class GameObjectScript : MonoBehaviour {
	public Camera cam1;
	public Camera cam2;
	public Camera cam3;
	public Camera cam4;
	public Camera cam5;
	public Camera cam6;
	public Camera cam7;
	public Camera cam8;

	public Transform cam4_tf;
	public Transform cam5_tf;
	public Transform cam6_tf;


	public Transform center;
//	public Transform rocket;
//	public Transform rocket_body;
	public Rigidbody rocket_rb;
	public Transform spacecraft1;
	public Transform spacecraft2;
	public GameObject rocket_GOBJ;
	public GameObject spacecraft1_GOBJ;
	public GameObject spacecraft2_GOBJ;
	public GameObject spacecraft2Flame_GOBJ;
	public GameObject rocketFlame_Main_GOBJ;
	public GameObject rocketFlame_Sub_GOBJ;
	public Transform rocketFlame_Main_tf;

	public GameObject AuxiRocket1;
	public GameObject AuxiRocket2;

	private float spacecraft1_RotationSpeed = 1.0f;
	private float spacecraft2_RotationSpeed = 10.0f;

	private Vector3 axis;
	public Transform GBJ;
	public Text cameraText;
	public Text statusText;
	public Text speedText;
	public Text distanceText;
	public Text heightText;

	private int stateFlag;  //0: Waiting for launching; 1: Launching ; 2: End Launching
	//public Vector3 force;
	private float launchSpeed = 200.0f;
	private float orbitRadius;
	private float earthRadius;
	private float orbitHeight;
	private float speedNum;
	private float rotateNum;
	private bool rotateReadyFlag;
	private bool auxiDropFlag;
	private bool rocketDropFlag;
	private bool rocketValidFlag;
	private bool spacecraft1ValidFlag;

	public Rigidbody enermytile;
	private int enermyNum = 20;
	private int enermyIter = 0;

	// Use this for initialization
	void Start () {
		cam1.enabled = true;
		cam2.enabled = false;
		cam3.enabled = false;
		cam4.enabled = false;
		cam5.enabled = false;
		cam6.enabled = false;
		cam7.enabled = false;
		cam8.enabled = false;


		axis = center.up;
		Vector3 Aircraft_posL = new Vector3 (0.0f, 0.0f, 25.0f);
		spacecraft2.position = center.TransformPoint (Aircraft_posL);
		spacecraft2.Rotate(new Vector3 (10f, 135f, 0f));

		stateFlag = 0;
		orbitRadius = (spacecraft2.position - center.position).magnitude;
		earthRadius = (rocket_GOBJ.transform.position - center.position).magnitude;
		orbitHeight = orbitRadius - earthRadius;
		speedNum = 0.0f;
		rotateNum = 0.0f;
		rotateReadyFlag = false;
		auxiDropFlag = false;
		rocketDropFlag = false;
		rocketValidFlag = true;
		spacecraft1ValidFlag = false;
		rocketFlame_Main_GOBJ.SetActive (false);
		rocketFlame_Sub_GOBJ.SetActive (false);
		spacecraft1_GOBJ.SetActive (false);
	
		cameraText.text = "Camera1: Overview";
		statusText.text = "Waiting Launching..";
		speedText.text = "Speed: N/A";
		distanceText.text = "Distance: N/A";
		heightText.text = "Height: 0.0f";
	}
	
	// Update is called once per frame
	void Update () {
		float distance = 0.1f;
		float spHeight = 0.0f;
		if (Input.GetKeyDown (KeyCode.Alpha1)) {
			cam1.enabled = true;
			cam2.enabled = false;
			cam3.enabled = false;
			cam4.enabled = false;
			cam5.enabled = false;
			cam6.enabled = false;
			cam7.enabled = false;
			cam8.enabled = false;
			cameraText.text = "Camera1: Overview";
		}
		if (Input.GetKeyDown (KeyCode.Alpha2)) {
			cam1.enabled = false;
			cam2.enabled = true;
			cam3.enabled = false;
			cam4.enabled = false;
			cam5.enabled = false;
			cam6.enabled = false;
			cam7.enabled = false;
			cam8.enabled = false;
			cameraText.text = "Camera2: Aircraft Back";
		}
		if (Input.GetKeyDown (KeyCode.Alpha3)) {
			cam1.enabled = false;
			cam2.enabled = false;
			cam3.enabled = true;
			cam4.enabled = false;
			cam5.enabled = false;
			cam6.enabled = false;
			cam7.enabled = false;
			cam8.enabled = false;
			cameraText.text = "Camera3: Aircraft Overview";
		}
		if (Input.GetKeyDown (KeyCode.Alpha4)) {
			cam1.enabled = false;
			cam2.enabled = false;
			cam3.enabled = false;
			cam4.enabled = true;
			cam5.enabled = false;
			cam6.enabled = false;
			cam7.enabled = false;
			cam8.enabled = false;
			if (stateFlag < 5d)
				cameraText.text = "Camera4: Rocket Front";
			else{
				cameraText.text = "Camera4: Spacecraft1 Top";
			}
		}
		if (Input.GetKeyDown (KeyCode.Alpha5)) {
			cam1.enabled = false;
			cam2.enabled = false;
			cam3.enabled = false;
			cam4.enabled = false;
			cam5.enabled = true;
			cam6.enabled = false;
			cam7.enabled = false;
			cam8.enabled = false;
			if (stateFlag < 5d)
				cameraText.text = "Camera5: Rocket Back";
			else{
				cameraText.text = "Camera5: Spacecraft1 Back";
			}

		}
		if (Input.GetKeyDown (KeyCode.Alpha6)) {
			cam1.enabled = false;
			cam2.enabled = false;
			cam3.enabled = false;
			cam4.enabled = false;
			cam5.enabled = false;
			cam6.enabled = true;
			cam7.enabled = false;
			cam8.enabled = false;
			if (stateFlag < 5d)
				cameraText.text = "Camera6: Rocket Overview";
			else{
				cameraText.text = "Camera6: Spacecraft1 Overview";
			}
		}
		if (Input.GetKeyDown (KeyCode.Alpha7)) {
			cam1.enabled = false;
			cam2.enabled = false;
			cam3.enabled = false;
			cam4.enabled = false;
			cam5.enabled = false;
			cam6.enabled = false;
			cam7.enabled = true;
			cam8.enabled = false;
			cameraText.text = "Camera7: Spacecraft2 Overview";
		}
		if (Input.GetKeyDown (KeyCode.Alpha8)) {
			cam1.enabled = false;
			cam2.enabled = false;
			cam3.enabled = false;
			cam4.enabled = false;
			cam5.enabled = false;
			cam6.enabled = false;
			cam7.enabled = false;
			cam8.enabled = true;
			cameraText.text = "Camera8: First Person View";
		}

		spacecraft2.RotateAround (center.position, axis, -spacecraft2_RotationSpeed * Time.deltaTime);

		if (rocketValidFlag == true) {
			axis = center.up;
			float height = (rocket_GOBJ.transform.position - center.position).magnitude - earthRadius;
			//float t_rotationSpeed = ((1.0f - (orbitHeight - height) / orbitHeight) * 35.0f) + spacecraft1_RotationSpeed;
			rocket_GOBJ.transform.RotateAround (center.position, axis, -spacecraft1_RotationSpeed * Time.deltaTime);

			//Waiting for Launching Command...
			if (stateFlag == 0 && (Input.GetButtonDown ("Fire1"))) {
				Vector3 speedVector = (rocket_GOBJ.transform.position - center.position).normalized * launchSpeed;
				rocket_rb.AddForce (speedVector);
				speedNum += 1.0f;
				rocketFlame_Main_GOBJ.SetActive (true);
				rocketFlame_Sub_GOBJ.SetActive (true);
				stateFlag = 1;
				statusText.text = "Launching..";
			}

			//Rotating and Launching...
			if (stateFlag == 1 ) {
				spacecraft1_RotationSpeed = 35.0f;
				if (rotateNum < 90.0f && rotateReadyFlag == false) {
					rocket_GOBJ.transform.Rotate (new Vector3 (0f, 0f, 1.0f));
					rotateNum += 1.0f;
				} else {
					rotateReadyFlag = true;	
				}
				if (height >= orbitHeight) {
					Vector3 speedVector = (rocket_GOBJ.transform.position - center.position).normalized * launchSpeed * speedNum * -1.0f;
					rocket_rb.AddForce (speedVector);

				}
				if (height >= orbitHeight && rotateReadyFlag == true) {
					stateFlag = 2;
					statusText.text = "Drop Auxi-Power..";
				}
			}

			//Wainting for Equator-Moving Command...
			if (stateFlag == 2 && Input.GetButtonDown ("Fire1") ) {
				Vector3 VecCen_Y = center.up;
				Vector3 VecCen_Z = center.forward;
				Vector3 VecCen_X = center.right;
				statusText.text = "Moving to Equator..";
				stateFlag = 3;
			}

			if (stateFlag >= 2 && auxiDropFlag == false) {
				float dropSpeed = 5.0f;
				Vector3 move1 =  ( AuxiRocket1.transform.position - center.position).normalized * (earthRadius-0.5f) + center.position ;
				Vector3 move2 =  ( AuxiRocket2.transform.position - center.position).normalized * (earthRadius-0.5f) + center.position ;
				AuxiRocket1.transform.position = Vector3.MoveTowards (AuxiRocket1.transform.position, move1, Time.deltaTime * dropSpeed);
				AuxiRocket2.transform.position = Vector3.MoveTowards (AuxiRocket2.transform.position, move2, Time.deltaTime * dropSpeed);
				if ((AuxiRocket1.transform.position - center.position).magnitude < earthRadius) {
					Destroy (AuxiRocket1);
					Destroy (AuxiRocket2);
					auxiDropFlag = true;
					statusText.text = "Waiting Eqt-MV CMD..";
				}

			}

			//Moving to Equator...
			if (stateFlag == 3 ) {
				Vector3 Rocket_posL = center.InverseTransformPoint (rocket_GOBJ.transform.position);
				float X_t = Rocket_posL.x;
				float Y_t = Rocket_posL.y;
				float Z_t = Rocket_posL.z;
				if (Y_t > 1.0f && Z_t!= 0) {
					Vector3 axis_posL = new Vector3 (1.0f, 0.0f, -1.0f * X_t / Z_t);
					Vector3 axis_posG = center.TransformPoint (axis_posL);
					rocket_GOBJ.transform.RotateAround (center.position, axis_posG, 0.1f);
					if (center.InverseTransformPoint(rocket_GOBJ.transform.position).y > Y_t )
						rocket_GOBJ.transform.RotateAround (center.position, axis_posG, -0.1f *2f );
				}
				if (Y_t <= 1.0f && Y_t > 0.0f && Z_t!= 0) {
					Vector3 axis_posL = new Vector3 (1.0f, 0.0f, -1.0f * X_t / Z_t);
					Vector3 axis_posG = center.TransformPoint (axis_posL);
					rocket_GOBJ.transform.RotateAround (center.position, axis_posG, 0.02f);
					if (center.InverseTransformPoint(rocket_GOBJ.transform.position).y > Y_t )
						rocket_GOBJ.transform.RotateAround (center.position, axis_posG, -0.02f *2f );
				}
				else if (Y_t <= 0.0f){
					statusText.text = "Waiting Joint CMD..";
					stateFlag = 4;
				}
			}
			//Wainting for Joint Command...
			if ( stateFlag == 4 && Input.GetButtonDown ("Fire1") ) {
				spacecraft1_GOBJ.SetActive (true);
				spacecraft1.parent = null;
				spacecraft1_RotationSpeed = 25.0f;
				spacecraft1.Rotate(new Vector3 (0f, 0f, 0f));

				cam4_tf.SetParent (spacecraft1);
				cam5_tf.SetParent (spacecraft1);
				cam6_tf.SetParent (spacecraft1);
				rocketFlame_Main_tf.SetParent (spacecraft1);
				rocketFlame_Sub_GOBJ.SetActive (false);
				spacecraft1ValidFlag = true;
				statusText.text = "Drop the rocket..";
				stateFlag = 5;
			}
			distance = (rocket_GOBJ.transform.position - spacecraft2.position).magnitude;
			spHeight = (rocket_GOBJ.transform.position - center.position).magnitude - earthRadius;
		}
		if (spacecraft1ValidFlag == true) {
			axis = center.up;

			spacecraft1.RotateAround (center.position, axis, -spacecraft1_RotationSpeed * Time.deltaTime);


			if (stateFlag >= 5 && rocketDropFlag == false) {
				float dropSpeed = 5.0f;
				Vector3 move1 =  ( rocket_GOBJ.transform.position - center.position).normalized * (earthRadius-0.5f) + center.position ;
				rocket_GOBJ.transform.position = Vector3.MoveTowards (rocket_GOBJ.transform.position, move1, Time.deltaTime * dropSpeed);
				if ((rocket_GOBJ.transform.position - center.position).magnitude < earthRadius) {
					Destroy (rocket_GOBJ);
					rocketDropFlag = true;
					rocketValidFlag = false;
					statusText.text = "Approaching..";
				}

			}

			distance = (spacecraft1.position - spacecraft2.position).magnitude;
			spHeight = (spacecraft1.position - center.position).magnitude - earthRadius;
			//Jointing....
			if (stateFlag == 5) {
				if (distance > 5.0f && distance <= 25.0f ) {
					spacecraft1_RotationSpeed = 10.0f;
					spacecraft2_RotationSpeed = 10.0f;
					spacecraft2Flame_GOBJ.SetActive(false);
					rocketFlame_Main_GOBJ.SetActive(false);
					spacecraft1.position = Vector3.MoveTowards (spacecraft1.position, spacecraft2.position, Time.deltaTime * 3.0f);
					statusText.text = "Off throttle..";
				}
				if (distance>2.0f && distance <= 5.0f) {
					spacecraft1_RotationSpeed = 10.0f;
					spacecraft2_RotationSpeed = 10.0f;
					spacecraft1.position = Vector3.MoveTowards (spacecraft1.position, spacecraft2.position, Time.deltaTime * 1.0f);
					statusText.text = "Jointing..";
				}
				if (distance <= 2.0f&&distance > 1.0f) {
					spacecraft1_RotationSpeed = 10.0f;
					spacecraft2_RotationSpeed = 10.0f;
					spacecraft1.position = Vector3.MoveTowards (spacecraft1.position, spacecraft2.position, Time.deltaTime * 0.5f);
					statusText.text = "Slow down speed..";
				}
				if (distance <= 1.0f&&distance > 0.5f) {
					spacecraft1_RotationSpeed = 10.0f;
					spacecraft2_RotationSpeed = 10.0f;
					spacecraft1.position = Vector3.MoveTowards (spacecraft1.position, spacecraft2.position, Time.deltaTime * 0.2f);
					statusText.text = "Slow down speed again..";
				}
				if (distance <= 0.5f&&distance > 0.1f) {
					spacecraft1_RotationSpeed = 10.0f;
					spacecraft2_RotationSpeed = 10.0f;
					spacecraft1.position = Vector3.MoveTowards (spacecraft1.position, spacecraft2.position, Time.deltaTime * 0.1f);
					statusText.text = "Keep attention..";
				}
				if (distance <= 0.1f&&distance > 0.001f) {
					spacecraft1_RotationSpeed = 10.0f;
					spacecraft2_RotationSpeed = 10.0f;
					spacecraft1.position = Vector3.MoveTowards (spacecraft1.position, spacecraft2.position, Time.deltaTime * 0.05f);
					statusText.text = "Connecting..";
				}
				if (distance <= 0.001f) {
					spacecraft1.SetParent (spacecraft2);
					spacecraft1_RotationSpeed = 0.0f;
					statusText.text = "Mission Complete!!";
					stateFlag = 6;
				}
			}

			if (stateFlag == 6 && Input.GetButtonDown ("Fire1")) {
				statusText.text = "UFO Invasion!!!";
				cam1.enabled = false;
				cam2.enabled = false;
				cam3.enabled = false;
				cam4.enabled = false;
				cam5.enabled = false;
				cam6.enabled = false;
				cam7.enabled = false;
				cam8.enabled = true;
				cameraText.text = "Camera8: First Person View";
				spacecraft2_RotationSpeed = 0.0f;
				stateFlag = 7;
			}
			if (stateFlag == 7) {

				while (enermyIter < enermyNum) {
					Vector3 enermyPosition = spacecraft2.position + spacecraft2.TransformVector(new Vector3 (Random.Range (100.0f, 1000.0f), Random.Range (-100.0f, 100.0f), Random.Range (-600.0f, 600.0f)) );
					Rigidbody enermy = (Rigidbody)Instantiate (enermytile, enermyPosition, transform.rotation);
					enermy.SendMessage ("SetSpacecraft", spacecraft2);
					enermyIter = enermyIter + 1;
				}

			}
				

		}
			
		//cameraText.text = "Camera1: Overview";
		if (cam2.enabled == true || cam3.enabled == true) {
			speedText.text = "Speed: 20.00";
		} else if (cam4.enabled == true || cam5.enabled == true || cam6.enabled == true) {
			speedText.text = "Speed: " + spacecraft1_RotationSpeed.ToString ();
		} else if (cam7.enabled == true) {
			speedText.text = "Speed: " + spacecraft2_RotationSpeed.ToString ();
		} else {
			speedText.text = "Speed: N/A";
		}
		heightText.text = "Height: " + spHeight.ToString();
		distanceText.text = "Distance: " + distance.ToString();
	}

}
