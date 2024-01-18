import {
	ChangeEventHandler, FocusEventHandler,
	FormEventHandler,
	MouseEventHandler,
	useEffect,
	useRef,
	useState,
} from 'react';
import { getFormValues, setFormValues } from '@theoryofnekomata/formxtra';
import { Button } from '~/components/Button';
import { FileButton } from '~/components/FileButton';
import * as FileModule from '~/modules/file';
import {ComboBox} from '~/components/ComboBox';

type SvgGroup = {
	id: string;
	name: string;
}

type KeyframeAttribute = {
	group: SvgGroup;
	translateX: number;
	translateY: number;
	rotate: number;
}

type Keyframe = {
	time: number;
	attributes: KeyframeAttribute[];
}

const useAnimationForm = () => {
	const [loadedFileName, setLoadedFileName] = useState<string>();
	const [loadedFileContents, setLoadedFileContents] = useState<string>();
	const [svgAnimations, setSvgAnimations] = useState<string[]>();
	const [svgGroups, setSvgGroups] = useState<SvgGroup[]>();
	const [isNewAnimationId, setIsNewAnimationId] = useState<boolean>();
	const [currentAnimationId, setCurrentAnimationId] = useState<string>();
	const [enabledGroupIds, setEnabledGroupIds] = useState<string[]>();
	const [savedSvgKeyframes, setSavedSvgKeyframes] = useState<Keyframe[]>();

	const svgPreviewRef = useRef<HTMLDivElement>(null);

	const loadSvgFile: ChangeEventHandler<HTMLInputElement> = async (e) => {
		const filePicker = e.currentTarget;
		if (!filePicker.files) {
			return;
		}
		const [file] = Array.from(filePicker.files);
		if (!file) {
			return;
		}
		setLoadedFileName(file.name);
		setIsNewAnimationId(false);
		const result = await FileModule.readFile(file);
		setLoadedFileContents(result);
		filePicker.value = ''
	}

	const selectNewAnimationId: FormEventHandler<HTMLInputElement> = (e) => {
		const target = e.currentTarget;
		setIsNewAnimationId(false);
		setCurrentAnimationId(e.currentTarget.value);
		setTimeout(() => {
			target.blur();
		});
	}

	const proposeNewAnimationId: FormEventHandler<HTMLInputElement> = () => {
		setIsNewAnimationId(true);
	}

	const cancelAnimationIdChange: MouseEventHandler<HTMLButtonElement> = (e) => {
		e.preventDefault();
		setIsNewAnimationId(false);
		setFormValues(e.currentTarget.form!, {
			animationId: currentAnimationId,
		})
	}

	const onLoaded = (loadedFileContents?: string, svgPreviewRefCurrent?: HTMLElement | null) => {
		if (!loadedFileContents) {
			return
		}

		if (!svgPreviewRefCurrent) {
			return
		}

		const svgPreviewRoot = svgPreviewRefCurrent.children[0];
		setSvgAnimations(['default']);
		setSvgGroups(
			Array
				.from(svgPreviewRoot.children)
				.map((g) => ({
					id: g.id,
					name: g.id,
				}))
		);
		setCurrentAnimationId('default');
		setEnabledGroupIds([]); // TODO - compute enabled groupIds based on current animation id
		setSavedSvgKeyframes([]); // TODO - load keyframes from file
	}

	useEffect(() => {
		onLoaded(loadedFileContents, svgPreviewRef.current);
	}, [loadedFileContents, svgPreviewRef.current]);

	const focusAnimationIdInput: FocusEventHandler<HTMLInputElement> = (e) => {
		e.currentTarget.value = '';
	}

	const blurAnimationIdInput: FocusEventHandler<HTMLInputElement> = (e) => {
		e.currentTarget.value = currentAnimationId as string;
		setIsNewAnimationId(false);
	}

	const addKeyframeAttribute: ChangeEventHandler<HTMLInputElement> = (e) => {
		const { currentTarget } = e;
		if (currentTarget.checked) {
			setEnabledGroupIds((oldEnabledGroupIds = []) => [...oldEnabledGroupIds, currentTarget.value]);
			return
		}
		setEnabledGroupIds((oldEnabledGroupIds = []) => oldEnabledGroupIds.filter((i) => i !== currentTarget.value));
	}

	const handleAttributeValueChange = (group: SvgGroup): ChangeEventHandler<HTMLInputElement> => (e) => {
		const { current: svgPreviewWrapper } = svgPreviewRef;
		if (!svgPreviewWrapper) {
			return;
		}

		const svgPreviewRoot = svgPreviewWrapper.children[0] as SVGSVGElement | null;
		if (!svgPreviewRoot) {
			return;
		}

		const targetGroup = Array.from(svgPreviewRoot.children).find(g => g.id === group.id) as SVGGraphicsElement;
		if (!targetGroup) {
			return;
		}

		const rotationCenterReference = Array.from(targetGroup.children).find(e => e.tagName === 'circle') as SVGCircleElement | undefined
		const rotationCenter = {
			cx: rotationCenterReference?.getAttribute?.('cx') ?? 0,
			cy: rotationCenterReference?.getAttribute?.('cy') ?? 0,
		};

		const form = e.currentTarget.form as HTMLFormElement
		const values = getFormValues(form, { forceNumberValues: true });
		const changed = Array.isArray(values.changed) ? values.changed : [values.changed];
		const changedIndex = changed.indexOf(group.id);
		const rotate = Array.isArray(values.rotate) ? values.rotate : [values.rotate];
		const rotateNumber = Array.isArray(values.rotateNumber) ? values.rotateNumber : [values.rotateNumber];
		const translateX = Array.isArray(values.translateX) ? values.translateX : [values.translateX];
		const translateY = Array.isArray(values.translateY) ? values.translateY : [values.translateY];
		const currentRotate = e.currentTarget.name.startsWith('rotate') ? e.currentTarget.valueAsNumber : rotate[changedIndex];
		const currentTranslateX = e.currentTarget.name.startsWith('translateX') ? e.currentTarget.valueAsNumber : translateX[changedIndex];
		const currentTranslateY = e.currentTarget.name.startsWith('translateY') ? e.currentTarget.valueAsNumber : translateY[changedIndex];
		if (e.currentTarget.name === 'rotate') {
			setFormValues(form, {
				rotateNumber: rotate,
			})
		} else if (e.currentTarget.name === 'rotateNumber') {
			setFormValues(form, {
				rotate: rotateNumber,
			})
		}
		targetGroup.setAttribute('transform', `translate(${currentTranslateX} ${currentTranslateY}) rotate(${currentRotate} ${rotationCenter.cx} ${rotationCenter.cy})`);
	}

	const handleFormSubmit: FormEventHandler<HTMLFormElement> = (e) => {
		e.preventDefault();
		const { submitter } = e.nativeEvent as unknown as { submitter: HTMLButtonElement };
		const values = getFormValues(e.currentTarget, { submitter, forceNumberValues: true });
		const changed = Array.isArray(values.changed) ? values.changed : [values.changed];
		const translateX = Array.isArray(values.translateX) ? values.translateX : [values.translateX];
		const translateY = Array.isArray(values.translateY) ? values.translateY : [values.translateY];
		const rotate = Array.isArray(values.rotate) ? values.rotate : [values.rotate];
		switch (values.action) {
		case 'updateTimeKeyframes':
			if ('changed' in values) {
				// TODO handle keyframes on different animations
				// TODO update SVG preview when updating current time
				setSavedSvgKeyframes((oldSavedSvgKeyframes = []) => {
					const newKeyframe = {
						time: values.time as number,
						attributes: changed.map((attribute, i) => ({
							group: {
								id: attribute,
								name: attribute,
							},
							translateX: translateX[i],
							translateY: translateY[i],
							rotate: rotate[i],
						})),
					};
					if (oldSavedSvgKeyframes.some((k) => k.time === values.time as number)) {
						return oldSavedSvgKeyframes.map((k) => k.time === values.time ? newKeyframe : k);
					}
					return [
						...oldSavedSvgKeyframes,
						newKeyframe,
					]
				})
				return;
			}
			setSavedSvgKeyframes((oldSavedSvgKeyframes = []) => oldSavedSvgKeyframes.filter(k => k.time !== values.time as number));
			return;
		default:
			break;
		}

		console.log(values);
	}

	const updateTime: ChangeEventHandler<HTMLInputElement> = (e) => {
		const value = e.currentTarget.valueAsNumber;
		const form = e.currentTarget.form as HTMLFormElement;
		const theSavedSvgKeyframes = savedSvgKeyframes ?? [];
		const sortedSavedSvgKeyframes = theSavedSvgKeyframes.sort((k1, k2) => k1.time - k2.time);
		const previousKeyframe =
			[...sortedSavedSvgKeyframes].reverse().find((k) => k.time <= value)
			?? sortedSavedSvgKeyframes[0];
		const nextKeyframe =
			sortedSavedSvgKeyframes.find((k) => k.time > value)
			?? sortedSavedSvgKeyframes.slice(-1)[0];
		const keyframe = savedSvgKeyframes?.find((k) => k.time === value);
		const hasKeyframeRange = (previousKeyframe || nextKeyframe);
		if (!keyframe) {
			setEnabledGroupIds(svgGroups?.map(g => g.id));
			// todo interpolate across individual keyframes
			//   get previous keyframe with respect to group
			//   get next keyframe with respect to group
			//   |----------------|
			//   |-----|

			const interpolatedKeyframe = previousKeyframe;
			const interpolatedValues = {
				changed: svgGroups?.map(() => false),
				translateX: svgGroups?.map((g) => {
					if (!hasKeyframeRange) {
						return 0;
					}
				})
			}
			setTimeout(() => {
				// todo set interpolation
				setFormValues(form, {
					changed: svgGroups?.map(() => false),
					translateX: svgGroups?.map(() => 0),
					translateY: svgGroups?.map(() => 0),
					rotate: svgGroups?.map(() => 0),
					rotateNumber: svgGroups?.map(() => 0),
				});
				setEnabledGroupIds([]);
			});
			return;
		}

		const changed = keyframe.attributes.map((a) => a.group.id);
		setEnabledGroupIds(changed);
		setTimeout(() => {
			setFormValues(form, {
				changed: changed,
				translateX: keyframe.attributes.map((a) => a.translateX),
				translateY: keyframe.attributes.map((a) => a.translateY),
				rotate: keyframe.attributes.map((a) => a.rotate),
				rotateNumber: keyframe.attributes.map((a) => a.rotate),
			});
		});
	}

	return {
		isNewAnimationId,
		svgAnimations,
		loadedFileName,
		loadedFileContents,
		loadSvgFile,
		svgPreviewRef,
		svgGroups,
		selectNewAnimationId,
		proposeNewAnimationId,
		cancelAnimationIdChange,
		focusAnimationIdInput,
		blurAnimationIdInput,
		addKeyframeAttribute,
		enabledGroupIds,
		handleAttributeValueChange,
		handleFormSubmit,
		savedSvgKeyframes,
		updateTime,
	}
}

const IndexPage = () => {
	const {
		loadedFileName,
		loadedFileContents,
		loadSvgFile,
		svgPreviewRef,
		svgGroups,
		svgAnimations,
		selectNewAnimationId,
		proposeNewAnimationId,
		isNewAnimationId,
		cancelAnimationIdChange,
		focusAnimationIdInput,
		blurAnimationIdInput,
		addKeyframeAttribute,
		enabledGroupIds,
		handleAttributeValueChange,
		handleFormSubmit,
		savedSvgKeyframes,
		updateTime,
	} = useAnimationForm();

	return (
		<form className="lg:fixed lg:w-full lg:h-full flex flex-col" onSubmit={handleFormSubmit}>
			<datalist id="rotationTickMarks">
				<option value="-360">-360</option>
				<option value="-270">-270</option>
				<option value="-180">-180</option>
				<option value="-90">-90</option>
				<option value="0">0</option>
				<option value="90">90</option>
				<option value="180">180</option>
				<option value="270">270</option>
				<option value="360">360</option>
			</datalist>
			<datalist id="timeTickMarks">
				{savedSvgKeyframes?.map((k) => (
					<option
						key={k.time}
						value={k.time}
					>
						{k.time}
					</option>
				))}
			</datalist>
			<div className="flex gap-4 px-4 py-2 items-center">
				<FileButton
					name="svgFile"
					accept="*.svg"
					onChange={loadSvgFile}
					className="whitespace-nowrap"
				>
					Load SVG File
				</FileButton>
				{
					loadedFileContents
					&& (
						<>
							<Button
								type="submit"
								className="whitespace-nowrap"
								name="action"
								value="saveSvgFile"
							>
								Save SVG File
							</Button>
							<div className="text-center w-full">
								{loadedFileName}
							</div>
						</>
					)
				}
			</div>
			{
				loadedFileContents
				&& (
					<div className="h-full flex flex-col">
						<div className="flex gap-4">
							<label className="block w-full">
								<span className="sr-only">
									Animation ID
								</span>
								<ComboBox
									name="animationId"
									defaultValue={svgAnimations?.[0] ?? ''}
									onOptionSelect={selectNewAnimationId}
									onCustomValueInput={proposeNewAnimationId}
									onFocus={focusAnimationIdInput}
									onBlur={blurAnimationIdInput}
									autoComplete="off"
								>
									{svgAnimations?.map(s => (
										<option key={s}>{s}</option>
									))}
								</ComboBox>
							</label>
							{
								isNewAnimationId && (
									<>
										<Button
											type="submit"
											name="action"
											value="renameCurrentAnimationId"
											className="whitespace-nowrap"
										>
											Rename
										</Button>
										<Button
											type="submit"
											name="action"
											value="createNewAnimation"
											className="whitespace-nowrap"
										>
											Create New
										</Button>
										<Button
											type="reset"
											className="whitespace-nowrap"
											onClick={cancelAnimationIdChange}
										>
											Cancel
										</Button>
									</>
								)
							}
						</div>
						<div
							className="flex gap-4 h-full"
						>
							<div
								className="overflow-auto bg-[black] relative w-full"
							>
								<div
									className="absolute p-4"
								>
									<div
										className="bg-[white] p-4"
										ref={svgPreviewRef}
										dangerouslySetInnerHTML={{ __html: loadedFileContents ?? '' }}
									/>
								</div>
							</div>
							<div
								style={{
									width: 700
								}}
							>
								{
									Array.isArray(svgGroups) && (
										<div className="grid grid-cols-10">
											<div className="col-span-4">
												Group
											</div>
											<div>
												X
											</div>
											<div>
												Y
											</div>
											<div className="col-span-4">
												Rotate
											</div>
											{svgGroups.map((group) => {
												return (
													<fieldset
														key={group.id}
														className="contents"
													>
														<legend className="sr-only">
															{group.name}
														</legend>
														<div className="col-span-4">
															<label
																className="whitespace-nowrap"
															>
																<input type="checkbox" name="changed" value={group.id} onChange={addKeyframeAttribute} />
																{' '}
																<span>
																	{group.name}
																</span>
															</label>
														</div>
														<label
															className="block"
														>
															<span className="sr-only">Translate X</span>
															<input
																className="w-full block text-right"
																type="number"
																name="translateX"
																min="-360" max="360"
																defaultValue="0"
																step="any"
																disabled={!enabledGroupIds?.includes(group.name)}
																onChange={handleAttributeValueChange(group)}
															/>
														</label>
														<label
															className="block"
														>
															<span className="sr-only">Translate Y</span>
															<input
																className="w-full block text-right"
																type="number"
																name="translateY"
																min="-360" max="360"
																defaultValue="0"
																step="any"
																disabled={!enabledGroupIds?.includes(group.name)}
																onChange={handleAttributeValueChange(group)}
															/>
														</label>
														<label
															className="block col-span-3"
														>
															<span className="sr-only">Rotate</span>
															<input
																className="w-full block"
																type="range"
																name="rotate"
																min="-360"
																max="360"
																defaultValue="0"
																step="5"
																disabled={!enabledGroupIds?.includes(group.name)}
																onChange={handleAttributeValueChange(group)}
																list="rotationTickMarks"
															/>
														</label>
														<label
															className="block"
														>
															<span className="sr-only">Rotate</span>
															<input
																className="w-full block text-right"
																type="number"
																name="rotateNumber"
																min="-360"
																max="360"
																defaultValue="0"
																step="5"
																disabled={!enabledGroupIds?.includes(group.name)}
																onChange={handleAttributeValueChange(group)}
															/>
														</label>
													</fieldset>
												)
											})}
										</div>
									)
								}
							</div>
						</div>
						<div className="flex gap-4 items-center px-4 py-2">
							<div
								className="w-16"
							>
								<input type="number" name="length" min="1" defaultValue="10" step="1" className="block w-full" />
							</div>
							<input
								className="w-full"
								list="timeTickMarks"
								type="range"
								name="time"
								min="0"
								max="10"
								defaultValue="0"
								step="1"
								onChange={updateTime}
							/>
							<Button
								type="submit"
								name="action"
								value="updateTimeKeyframes"
								className="whitespace-nowrap"
							>
								Update Current Time
							</Button>
						</div>
					</div>
				)
			}
		</form>
	);
}

export default IndexPage;
